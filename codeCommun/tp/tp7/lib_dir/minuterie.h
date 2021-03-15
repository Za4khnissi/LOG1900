#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000

namespace timer
{
    void partirMinuterie(uint16_t duree, uint16_t debut);
    void initialisationMinuterie (uint8_t valeurDDRx, uint8_t valeurDDRy, volatile uint8_t* DDRx, volatile uint8_t* DDRy);
} // namespace name
