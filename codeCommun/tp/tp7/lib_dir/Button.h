#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef BUTTON_H
#define BUTTON_H

class Button
{
private:
    enum PseudoPin {
        pseudoPD2 = 2,
        pseudoPD3,
        pseudoPB2
    };

    enum InterruptSenseControl {
        LOW_LEVEL,
        ANY_EDGE,
        FALLING_EDGE,
        RISING_EDGE
    };
public:
    Button(volatile uint8_t &DDRx, uint8_t pin);
    Button(volatile uint8_t *DDRx[], uint8_t pins[], uint8_t nbButtons);
    ~Button();
    void initialization(PseudoPin pseudoPin, InterruptSenseControl ics);
    bool debounce(uint8_t &pin, uint8_t buttonMask);
};

#endif //BUTTON_H