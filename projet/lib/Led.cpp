#include "Led.h"

Led::Led()
{
}
// 0000 0110
Led::~Led()
{
}

void Led::setLedColor(volatile uint8_t *port, uint8_t color)
{
    if (color == AMBRE)
    {
        if(val)
            *port |= GREEN;
        else {
            *port |= RED;
        }
        val = !val;
    }
    else
        *port = color;  
}