#define F_CPU 8000000UL
#include <avr/io.h>

#ifndef UART_H
#define UART_H

void initialisationUART ( void );
void transmissionUART ( uint8_t donnee );

#endif