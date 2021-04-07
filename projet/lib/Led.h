#ifndef LED_H
#define LED_H

#include <avr/io.h>

#define OFF   0
#define GREEN 1
#define RED   2
#define AMBRE 3

class Led
{
private:
    bool val = false;

public:
    Led();
    ~Led();

    void setLedColor(volatile uint8_t *port, uint8_t color);
};

#endif //LED_H

