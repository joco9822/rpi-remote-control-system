#include <wiringPi.h>
#include <softTone.h> 
#include "../include/common.h"  

#define BUZZER_PIN 29   /* WiringPi 핀 번호 */

int notes[] = { 
    391, 391, 440, 440, 391, 391, 329, 329, 
    391, 391, 329, 329, 293, 293, 293, 0, 
    391, 391, 440, 440, 391, 391, 329, 329, 
    391, 329, 293, 329, 261, 261, 261, 0
};

int buzzer_init() {
    if (softToneCreate(BUZZER_PIN) != 0) {
        return -1;
    }
    return 0; 
}

int buzzer_control(int data) {
    if (data == 0) { /* 음악 끄기 */
        softToneWrite(BUZZER_PIN, 0);
    }
    else if (data == 1) { /* 음악 켜기 */
        int total = sizeof(notes) / sizeof(notes[0]); 
        for (int i = 0; i < total; i++) {
            softToneWrite(BUZZER_PIN, notes[i]);
            delay(300); 
        }
        softToneWrite(BUZZER_PIN, 0);
    }
    else {
        return -1; 
    }
    return 0; 
}