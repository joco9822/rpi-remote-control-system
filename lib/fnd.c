#include <wiringPi.h>
#include "common.h"

/* 7447 BCD 디코더 입력 핀 정의 (WiringPi 핀 번호) */
#define PIN_FND_A 4  /* LSB (2^0) */
#define PIN_FND_B 1  /* (2^1) */
#define PIN_FND_C 16 /* (2^2) */
#define PIN_FND_D 15 /* MSB (2^3) */

int fnd_init() {
    /* FND 핀을 출력 모드로 설정 */
    pinMode(PIN_FND_A, OUTPUT);
    pinMode(PIN_FND_B, OUTPUT);
    pinMode(PIN_FND_C, OUTPUT);
    pinMode(PIN_FND_D, OUTPUT);

    /* 초기 상태 0 표시 */
    digitalWrite(PIN_FND_A, LOW);
    digitalWrite(PIN_FND_B, LOW);
    digitalWrite(PIN_FND_C, LOW);
    digitalWrite(PIN_FND_D, LOW);
    return 0; 
}

int fnd_control(int data) {
    if (data < 0 || data > 9) {
        return -1; /* 유효하지 않은 숫자 */
    }

    /* BCD 값 설정 */
    digitalWrite(PIN_FND_A, (data & 0x01) ? HIGH : LOW); /* 2^0 */
    digitalWrite(PIN_FND_B, (data & 0x02) ? HIGH : LOW); /* 2^1 */
    digitalWrite(PIN_FND_C, (data & 0x04) ? HIGH : LOW); /* 2^2 */
    digitalWrite(PIN_FND_D, (data & 0x08) ? HIGH : LOW); /* 2^3 */

    return 0; 
}