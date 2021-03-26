#define F_CPU 8000000UL
#include <avr/io.h>

#ifndef UART_H
#define UART_H


class UART {
public:
    UART();
    ~UART();
    void transmissionUART(uint8_t data);
    uint8_t receive();

};

#endif // UART_H