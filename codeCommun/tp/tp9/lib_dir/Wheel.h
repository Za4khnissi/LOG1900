#ifndef WHEEL_H
#define WHEEL_H

#include <avr/io.h>
#include "PWM.h"

class Wheel
{
private:
    bool fwd = false;
    
public:
    Wheel();
    ~Wheel();
    void forward(uint8_t ocr1a, uint8_t ocr1b);
    void backward(uint8_t ocr1a, uint8_t ocr1b);
    void turnLeft(uint8_t ocr1a, uint8_t ocr1b);
    void turnRight(uint8_t ocr1a, uint8_t ocr1b);   
    void stop(uint8_t ocr1a, uint8_t ocr1b);
};

Wheel::Wheel()
{
}

Wheel::~Wheel()
{
}


#endif //WHEEL_H
