#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define OFF   0
#define GREEN 1
#define RED   2
#define AMBRE 3

#define THRESHOLD 4
#define JUMP      10

enum PinState {
    LOW, 
    HIGH 
};


void setPinState(volatile uint8_t &port, uint8_t pins[], PinState pinState[], uint8_t nbPins)
{
    for(uint8_t i = 0; i < nbPins; i++)
    {
        if (pins[i] > 7) {
            continue; // Numéro de pin invalide, prochaine pin
        }
        port = (pinState[i] == PinState::HIGH) ? port | _BV(pins[i]) : port & ~_BV(pins[i]);
    }
}

void setDELColor(uint8_t &port, uint8_t color)
{
    if (color == AMBRE)
    {
        static bool val = false;
        if(val)
            port = GREEN;
        else {
            port = RED;
        }
        val = !val;
    }
    else
        port = color;
}

void dynamic_delay_ms(uint16_t delay)
{
    if (delay < THRESHOLD)
    {
        for (uint16_t i = 0; i < delay; i++)
        {
            _delay_ms(1);
        }
    } else 
    {
        uint16_t jump = delay >> 2;
        for (uint16_t i = 0; i < delay; i+= jump)
        {
            for (uint8_t j = 0; j < jump / 10; j++)
            {
                _delay_ms(10);
            }
            
        }
    }

}
