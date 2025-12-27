#include <wiringPi.h>
#include <softTone.h> 
#include "common.h"  

#define PIN_BUZZER 29   /* WiringPi 핀 번호 */

int buzzer_init() {
    if (softToneCreate(PIN_BUZZER) != 0) {
        return -1;
    }
    return 0; 
}

int buzzer_control(int data) {
    softToneWrite(PIN_BUZZER, data);
    return 0; 
}