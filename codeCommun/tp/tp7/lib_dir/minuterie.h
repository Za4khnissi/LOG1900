#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000

namespace timer
{
    void partirMinuterie(uint16_t duree, uint16_t debut);
    void initialisationMinuterie (uint8_t valeurDDRA, uint8_t valeurDDRD);
} // namespace name
