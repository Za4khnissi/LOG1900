#define F_CPU 8000000UL
#include <avr/io.h>

#ifndef UART_H
#define UART_H

void initializeUART ( void );
void transmissionUART ( uint8_t data );

#endif