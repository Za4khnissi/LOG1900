#include "Del.h"

void Del::setDelColor(volatile uint8_t *port, uint8_t color)
{
    if (color == DelColor::AMBRE)
    {
        if(val)
            *port |= DelColor::GREEN;
        else {
            *port |= DelColor::RED;
        }
        val = !val;
    }
    else
        *port |= color;
}