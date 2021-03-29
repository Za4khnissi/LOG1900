#include "Motor.h"


Motor::Motor()
{
}

Motor::~Motor()
{
}

void Motor::forward(uint8_t ocr1a, uint8_t ocr1b)
{
    Motor::fwd = true;
    PORTC |= _BV(PC0);
    PORTC |= _BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);
}
void Motor::backward(uint8_t ocr1a, uint8_t ocr1b)
{
    Motor::fwd = false;
    PORTC &= ~_BV(PC0);
    PORTC &= ~_BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);

}

void Motor::turnLeft(uint8_t ocr1a, uint8_t ocr1b)
{
    if(Motor::fwd) 
    {
        PORTC |= _BV(PC0);
    }
    else {
        PORTC &= ~_BV(PC0);
    }
    //PORTC |= _BV(PC1);
    PWM::adjustPWM(ocr1a, ocr1b);

}

void Motor::turnRight(uint8_t ocr1a, uint8_t ocr1b)
{
    if(Motor::fwd) 
    {
        PORTC |= _BV(PC1);
    }
    else {
        PORTC &= ~_BV(PC1);
    }
    PWM::adjustPWM(ocr1a, ocr1b);

}   
void Motor::stop(uint8_t ocr1a, uint8_t ocr1b)
{
    PWM::adjustPWM(0, 0);
}