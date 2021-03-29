#ifndef WHEEL_H
#define WHEEL_H

#include <avr/io.h>
#include "PWM.h"

class Motor
{
private:
    bool fwd = false;
    
public:
    Motor();
    ~Motor();
    void forward(uint8_t ocr1a, uint8_t ocr1b);
    void backward(uint8_t ocr1a, uint8_t ocr1b);
    void turnLeft(uint8_t ocr1a, uint8_t ocr1b);
    void turnRight(uint8_t ocr1a, uint8_t ocr1b);   
    void stop(uint8_t ocr1a, uint8_t ocr1b);
};



#endif //WHEEL_H
