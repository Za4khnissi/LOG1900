#include "Del.h"

Del::Del()
{
}

Del::~Del()
{
}

void Del::setDelColor(volatile uint8_t *port, uint8_t color)
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
        *port |= color;
}