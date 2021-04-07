#include "PWM.h"

void PWM::adjustPWM(uint8_t ocr1a, uint8_t ocr1b) {

    OCR1A = ocr1a;
    OCR1B = ocr1b;
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    TCCR1B = (1 << CS11);
    TCCR1C = 0;
    
}