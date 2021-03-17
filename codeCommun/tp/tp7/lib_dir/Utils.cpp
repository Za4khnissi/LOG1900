#include "Utils.h"

void Utils::setPinState(volatile uint8_t &port, uint8_t pins[], PinState pinState[], uint8_t nbPins)
{
    for(uint8_t i = 0; i < nbPins; i++)
    {
        if (pins[i] > 7) {
            continue; // Numéro de pin invalide, prochaine pin
        }
        port = (pinState[i] == PinState::HIGH) ? port | _BV(pins[i]) : port & ~_BV(pins[i]);
    }
}

void Utils::setDELColor(uint8_t &port, uint8_t color)
{
    if (color == AMBRE)
    {
        static bool val = false;
        if(val)
            port = GREEN;
        else{
            port = RED;
        }
        val = !val;
    }
    else
        port = color;
}

void Utils::dynamic_delay_us(uint16_t delay)
{
    if(delay < THRESHOLD)
    {
        _delay_ms(THRESHOLD);
    } else 
    {
        for (uint16_t i = 0; i < delay / 10; i++)
        {
            _delay_ms(JUMP);
        }
    }

}


