#define F_CPU 8000000UL
#include <avr/io.h>

#ifndef PWM_H
#define PWM_H

namespace PWM
{
        void adjustPWM(uint8_t duration);
}


#endif
