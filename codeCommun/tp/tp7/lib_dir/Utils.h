#include <avr/io.h>

enum PinState {
    LOW, 
    HIGH 
};

namespace Utils
{
    void setPinState(volatile uint8_t &port, uint8_t pins[], PinState pinState[], uint8_t nbPins);
    void setDELColor(uint8_t &port, uint8_t color);
}