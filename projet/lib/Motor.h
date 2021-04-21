#ifndef WHEEL_H
#define WHEEL_H

#include <avr/io.h>
#include "PWM.h"

class Motor
{
private:
    bool fwd = true;
    
public:
    Motor();
    ~Motor();
    void forward(uint8_t ocr1a, uint8_t ocr1b);
    void backward(uint8_t ocr1a, uint8_t ocr1b);
    void turnLeft();
    void turnRight();   
    void stop();
};

#endif //WHEEL_H

