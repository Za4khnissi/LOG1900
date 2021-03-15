#include "PWM.h"


void PWM::ajusterPWM(uint8_t duree){

    OCR1A = duree;
    OCR1B = duree;
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    TCCR1B = (1 << CS11);
    TCCR1C = 0;

}