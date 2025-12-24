#include <wiringPi.h>
#include "common.h"

#define PIN_CDS 14   /* WiringPi 핀 번호 */

int cds_init() {
    pinMode(PIN_CDS, INPUT); /* CDS 핀을 입력 모드로 설정 */
    return 0; 
}

int cds_read() {
    return digitalRead(PIN_CDS); 
}

/* 회로도 및 동작 설명 
*
* 배선: 
    3.3V -- 10k옴 -- 핀 -- 조도센서 -- GND
*
* 동작 원리:
* - 어두울 때 : 조도 센서 저항 커져 분압에 의해 GPIO 전압이 높아짐 -> HIGH
* - 밝을 때 : 조도 센서 저항 낮아져 분압에 의해 GPIO 전압이 낮아짐 -> LOW
*
* 참고:
* - 라즈베리파이의 GPIO는 디지털 입력 핀의 HIGH/LOW를 특정 threshold 전압을 기준으로 판별함 
*/