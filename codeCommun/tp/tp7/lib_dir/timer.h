#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>

namespace timer
{
    void partirMinuterie(uint16_t duree, uint16_t debut);
    void initialisationMinuterie (uint8_ entree, uint8_t sortie);
} // namespace name
