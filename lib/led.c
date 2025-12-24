#include <wiringPi.h>
#include <softPwm.h>
#include "../include/common.h"

#define LED_PIN 26   /* WiringPi 핀 번호 */

int led_init() {
    pinMode(LED_PIN, OUTPUT); /* LED 핀을 출력 모드로 설정 */
    softPwmCreate(LED_PIN, 0, 100); /* 소프트 PWM 초기화 (0~100 범위) */

    softPwmWrite(LED_PIN, 100); /* 초기 상태: LED 끄기 */
    return 0; 
}

int led_control(int data) {
    if (data == 0) { /* LED 끄기 */
        softPwmWrite(LED_PIN, 100);
    }
    else if (data == 1) { /* LED 켜기 */
        softPwmWrite(LED_PIN, 0);
    }
    else if (data >= 2 && data <= 100) { /* LED 밝기 조절 */ 
        softPwmWrite(LED_PIN, 100 - data);
    }
    else {
        return -1; 
    }
    return 0; 
}
