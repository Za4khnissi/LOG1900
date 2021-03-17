#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

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

    enum ButtonDDR {
        pseudoDDRA,
        pseudoDDRB,
        pseudoDDRC,
        pseudoDDRD
    };
    enum InterruptSenseConrol {
        LOW_LEVEL,
        ANY_EDGE,
        FALLING_EDGE,
        RISING_EDGE
    };
public:
    Button(ButtonDDR buttonDDR, uint8_t pin);
    Button(ButtonDDR buttonDDR[], uint8_t pins[], uint8_t nbButtons);
    ~Button();
    void initialization(PseudoPin pseudoPin, InterruptSenseConrol ics);
    bool debounce(uint8_t &pin, uint8_t buttonMask);
};

#endif BUTTON_H