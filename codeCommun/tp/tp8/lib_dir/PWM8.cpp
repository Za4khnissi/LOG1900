#include "PWM8.h"


void PWM::adjustPWM(uint8_t duration){

    OCR1A = duration;
    OCR1B = duration;
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    TCCR1B = (1 << CS11);
    TCCR1C = 0;
}