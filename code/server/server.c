#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <wiringPi.h>
#include "common.h"

#define TOTAL_NOTES 32
#define BUZZER_ALRAM 340 
int notes[] = { 
    391, 391, 440, 440, 391, 391, 329, 329, 
    391, 391, 329, 329, 293, 293, 293, 0, 
    391, 391, 440, 440, 391, 391, 329, 329, 
    391, 329, 293, 329, 261, 261, 261, 0
};

/* 전역 변수 */
int cds_flag = OFF;
int buz_flag = OFF; 
int fnd_count = 0;
int fnd_flag = OFF;
device_ctl_t led_ctl, buz_ctl, fnd_ctl;
device_read_t cds_ctl;

/* [스레드 1] 조도 센서 -> LED */
void* cds_thread(void* arg) {
    while(1) {
        if (cds_flag == ON) {
            if (cds_ctl()) {
                led_ctl(ON);  // 어두우면 켬
            } else {
                led_ctl(OFF); // 밝으면 끔
            }
        }
        delay(200);
    }
    return NULL;
}

/* [스레드 2] 부저 연주 ON/OFF */
void* buz_thread(void* arg) {
    while (buz_flag == ON) {  // 반복 재생 
        for (int i = 0; i < TOTAL_NOTES; i++) {  // 한 곡 재생
            if (buz_flag == OFF) {
                break;
            }
            buz_ctl(notes[i]); 
            delay(300);
        }
        if (buz_flag == ON) delay(500); // 다음 곡 연주 전, 0.5초 대기
    }
    buz_ctl(OFF);  // 확실한 종료 
    return NULL;
}

/* [스레드 3] 7세그먼트 카운트다운 */
void* fnd_thread(void* arg) {
    fnd_flag = ON;
    for (int i = fnd_count; i >= 0; i--) {
        if (fnd_flag == OFF) {
            break;
        }
        fnd_ctl(i);
        if (i == 0) {  // 0 도달 시 부저 울림
            buz_ctl(BUZZER_ALRAM); 
            delay(500); 
            buz_ctl(OFF);
        }
        delay(1000);
    }
    fnd_flag = OFF;
    return NULL;
}

int main() {
    /* TCP - 변수 선언 */
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    Protocol packet;             // 서버 <-> 클라이언트 통신 구조체 (defined in common.h)

    /* 동적 라이브러리 - 변수 선언 */
    void *h_led, *h_buz, *h_cds, *h_fnd;
    device_init_t led_init, buz_init, cds_init, fnd_init;
    device_ctl_t led_ctl, buz_ctl, fnd_ctl;
    device_read_t cds_ctl;       // cds 센서는 device_read_t 타입 사용 

    /* 장치 스레드 변수 선언 */
    pthread_t tid_cds, tid_buz, tid_fnd;

    /* WiringPi - 초기화 */
    if (wiringPiSetup() == -1) return 1;

    /* 동적 라이브러리 - 라이브러리(.so) 로드 및 함수 매핑 */
    h_led = dlopen("./lib/libled.so", RTLD_LAZY);
    h_buz = dlopen("./lib/libbuzzer.so", RTLD_LAZY);
    h_cds = dlopen("./lib/libcds.so", RTLD_LAZY);
    h_fnd = dlopen("./lib/libfnd.so", RTLD_LAZY);

    if (!h_led || !h_buz || !h_cds || !h_fnd) {
        printf("[Error in server.c] 1. 동적 라이브러리 로드에 실패하였습니다.\n");
        return 1;
    }

    led_init = (device_init_t)dlsym(h_led, "led_init");
    led_ctl = (device_ctl_t)dlsym(h_led, "led_control");
    buz_init = (device_init_t)dlsym(h_buz, "buzzer_init");
    buz_ctl = (device_ctl_t)dlsym(h_buz, "buzzer_control");
    cds_init = (device_init_t)dlsym(h_cds, "cds_init");
    cds_ctl = (device_read_t)dlsym(h_cds, "cds_read");
    fnd_init = (device_init_t)dlsym(h_fnd, "fnd_init");
    fnd_ctl = (device_ctl_t)dlsym(h_fnd, "fnd_control");

    if (led_init) led_init();
    if (buz_init) buz_init();
    if (cds_init) cds_init();
    if (fnd_init) fnd_init();

    /* 장치 스레드 생성 (= 작업 시작) */
    pthread_create(&tid_cds, NULL, cds_thread, NULL);
    pthread_create(&tid_buz, NULL, buz_thread, NULL);
    pthread_create(&tid_fnd, NULL, fnd_thread, NULL);

    /* TCP - 소켓 설정 */
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(5100);

    /* TCP - 서버 소켓 바인딩 및 리스닝 */
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) return 1;
    if (listen(serv_sock, 5) == -1) return 1;

    while (1) {
        /* TCP - 클라이언트 연결 수락 */
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if (clnt_sock == -1) {
            printf("[Error in server.c] 2. 클라이언트 Accept 실패하였습니다. 연결을 재시도합니다.\n");
            continue;
        }

        while (read(clnt_sock, &packet, sizeof(packet)) > 0) {
            switch (packet.deviceid) {
                case DEV_LED:
                    if (led_ctl) led_ctl(packet.data);
                    break;
                case DEV_BUZZER:
                    if (packet.data == ON) {
                        pthread_create(&tid_buz, NULL, buz_thread, NULL);
                        pthread_detach(tid_buz);
                    } else {
                        if (buz_ctl) buz_ctl(OFF);
                    }
                    break;
                case DEV_CDS:
                    cds_flag = packet.data; // ON/OFF 플래그 설정
                    break;
                case DEV_FND:
                    if (packet.data == -1) fnd_flag = OFF; // 9번 메뉴: 중단
                    else {
                        fnd_count = packet.data;
                        pthread_create(&tid_fnd, NULL, fnd_thread, NULL);
                        pthread_detach(tid_fnd);
                    }
                    break;
            }
        }

        /* TCP - 클라이언트 소켓 닫기 */
        close(clnt_sock);
    }

    /* TCP - 서버 소켓 닫기 */
    close(serv_sock);
    return 0;
}