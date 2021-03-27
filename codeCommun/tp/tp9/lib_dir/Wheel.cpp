#include "Wheel.h"

void Wheel::forward(uint8_t ocr1a, uint8_t ocr1b)
{
    Wheel::fwd = true;
    PORTC |= _BV(PC0);
    PORTC |= _BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);
}
void Wheel::backward(uint8_t ocr1a, uint8_t ocr1b)
{
    Wheel::fwd = false;
    PORTC &= ~_BV(PC0);
    PORTC &= ~_BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);

}
void Wheel::turnLeft(uint8_t ocr1a, uint8_t ocr1b)
{
    if(Wheel::fwd) 
    {
        PORTC |= _BV(PC0);
    }
    else {
        PORTC &= ~_BV(PC0);
    }
    //PORTC |= _BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);

}
void Wheel::turnRight(uint8_t ocr1a, uint8_t ocr1b)
{
    if(Wheel::fwd) 
    {
        PORTC |= _BV(PC1);
    }
    else {
        PORTC &= ~_BV(PC1);
    }
    PWM::adjustPWM(ocr1a, ocr1b);

}   
void Wheel::stop(uint8_t ocr1a, uint8_t ocr1b)
{
    PWM::adjustPWM(0, 0);
}