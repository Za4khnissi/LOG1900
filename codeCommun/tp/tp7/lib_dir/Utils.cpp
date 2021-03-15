#include <avr/io.h> 
#include "Utils.h"

#define GREEN 1
#define RED 2
#define AMBRE 3

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

void debounce();

/*


*/