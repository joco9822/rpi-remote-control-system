#include <stdio.h>
#include <wiringPi.h>  
#include <dlfcn.h>     /* dlopen, dlsym, dlclose, dlerror */
#include "common.h"   

int main() {
    void *handle;
    device_init_t led_init_func;      
    device_ctl_t led_control_func; 

    wiringPiSetup( ); 		/* wiringPi 초기화 */

    /* 1. 라이브러리 파일 열기 */
    /* dlopen은 하드디스크의 .so 파일을 프로세스의 메모리 공간으로 읽어온다. */
    handle = dlopen("../lib/libled.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "라이브러리 로드 실패: %s\n", dlerror());
        return 1;
    }

    /* 2. 함수 주소 찾기 */
    /* dlsym은 메모리에 로드된 라이브러리에서 "문자열"과 일치하는 함수의 시작 주소를 찾는다. */
    led_init_func = (device_init_t)dlsym(handle, "led_init");         /* */
    led_control_func = (device_ctl_t)dlsym(handle, "led_control"); /* */

    /* 3. 함수 실행 */
    if (led_init_func) {
        printf("LED 초기화 실행\n");
        led_init_func(); /* led.c의 led_init() 호출 */
    }

    if (led_control_func) {
        printf("LED 켜기 명령 전송\n");
        led_control_func(50); /* led.c의 led_control(1) 호출 */
    }

    /* 4. 라이브러리 닫기 */
    /* 사용이 끝난 라이브러리를 메모리에서 해제한다. */
    dlclose(handle);

    while(1) delay(50);

    return 0;
}