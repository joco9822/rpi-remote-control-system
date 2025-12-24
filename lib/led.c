#include <wiringPi.h>
#include <softPwm.h>
#include "common.h"

#define PIN_LED 26   /* WiringPi 핀 번호 */

int led_init() {
    pinMode(PIN_LED, OUTPUT); /* LED 핀을 출력 모드로 설정 */
    if (softPwmCreate(PIN_LED, 0, 100) != 0) { /* 소프트 PWM 초기화 (0~100 범위) */
        return -1;
    }

    softPwmWrite(PIN_LED, 100); /* 초기 상태: LED 끄기 */
    return 0; 
}

int led_control(int data) {
    if (data == 0) { /* LED 끄기 */
        softPwmWrite(PIN_LED, 100);
    }
    else if (data == 1) { /* LED 켜기 */
        softPwmWrite(PIN_LED, 0);
    }
    else if (data >= 2 && data <= 100) { /* LED 밝기 조절 */ 
        softPwmWrite(PIN_LED, 100 - data);
    }
    else {
        return -1; 
    }
    return 0; 
}
