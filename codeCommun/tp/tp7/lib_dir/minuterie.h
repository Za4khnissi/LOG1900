#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000

namespace timer
{
    void setPrescaler(unsigned int prescaler);
    void startTimer(uint16_t duree, uint16_t start, unsigned int prescaler);
    void initializeTimer(uint8_t valueDDRx, uint8_t valueDDRy, volatile uint8_t* DDRx, volatile uint8_t* DDRy);
} // namespace name
