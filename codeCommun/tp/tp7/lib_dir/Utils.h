#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


enum PinState {
    LOW, 
    HIGH 
};

namespace Utils
{
    void setPinState(volatile uint8_t &port, uint8_t pins[], PinState pinState[], uint8_t nbPins);
    void setDELColor(uint8_t &port, uint8_t color);
    bool debounce(uint8_t &pin, uint8_t pressedButton);
}