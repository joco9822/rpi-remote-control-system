#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.h"

void display_menu() {
    printf("\n[ Device Control Menu ]\n");
    printf("1. LED ON\n");
    printf("2. LED OFF\n");
    printf("3. Set Brightness\n");
    printf("4. BUZZER ON (play melody)\n");
    printf("5. BUZZER OFF (stop)\n");
    printf("6. SENSOR ON (감시 시작)\n");
    printf("7. SENSOR OFF (감시 종료)\n");
    printf("8. SEGMENT DISPLAY (숫자 표시 후 카운트다운)\n");
    printf("9. SEGMENT STOP (카운트다운 중단)\n");
    printf("0. Exit\n");
    printf("Select: ");
}

int main(int argc, char **argv) {
    /* TCP - 변수 선언 */
    int sock;
    struct sockaddr_in serv_addr;
    Protocol packet;         // 서버 <-> 클라이언트 통신 구조체 (defined in common.h)
    int choice;              // 사용자 입력: 메뉴 선택

    if (argc != 2) {
        printf("[Error in client.c] 0. 서버 IP 인자가 없습니다.\n");  // client 프로그램은 서버 IP를 인자로 받음
        exit(1);
    }

    /* TCP - 소켓 설정 */
    sock = socket(PF_INET, SOCK_STREAM, 0);     // 도메인(IPv4), 타입(TCP)
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);  // 서버 IP
    serv_addr.sin_port = htons(5100);

    /* TCP - 서버 연결 요청 */
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("[Error in client.c] 1. TCP 서버 연결에 실패하였습니다.\n");
        exit(1);
    }

    while (1) {
        /* 사용자 인터페이스 */
        display_menu();
        scanf("%d", &choice);

        /* 사용자 입력 처리 - 0이면 종료 */
        if (choice == 0) break;

        /* 사용자 입력 처리 - 1~9 메뉴 기능 수행 */
        memset(&packet, 0, sizeof(packet));
        switch (choice) {
            /* LED ON/OFF */
            case 1: packet.deviceid = DEV_LED; packet.data = ON; break;
            case 2: packet.deviceid = DEV_LED; packet.data = OFF; break;
            /* LED 밝기 조절 */
            case 3: 
                packet.deviceid = DEV_LED;
                printf("LED 밝기를 입력해주세요 (2-100): "); 
                scanf("%d", &packet.data);
                //if (packet.data < 2 || packet.data > 100) 
                break;
            /* BUZZER ON/OFF */
            case 4: packet.deviceid = DEV_BUZZER; packet.data = ON; break;
            case 5: packet.deviceid = DEV_BUZZER; packet.data = OFF; break;
            /* CDS SENSOR 감시 모드 ON/OFF */
            case 6: packet.deviceid = DEV_CDS; packet.data = ON; break;
            case 7: packet.deviceid = DEV_CDS; packet.data = OFF; break;
            /* FND 카운트다운/중단 */
            case 8:
                packet.deviceid = DEV_FND;
                printf("카운트 다운할 숫자를 입력해주세요 (0-9): ");
                scanf("%d", &packet.data);
                break;
            case 9:
                packet.deviceid = DEV_FND;
                packet.data = -1; // -1을 '중단' 신호로 정의한다.
                break;
            /* 잘못된 입력 처리 */
            default: 
                printf("숫자를 잘못 입력하였습니다. 메뉴의 숫자를 선택해주세요. (0-9)\n"); continue;
        }

        /* TCP - 서버로 패킷 전송 */
        write(sock, &packet, sizeof(packet));
    }

    /* TCP - 클라이언트 소켓 닫기 (프로세스 종료 시) */
    close(sock);
    return 0;
}