#ifndef UART_H
#define UART_H

#include <avr/io.h>

class UART {
public:
    UART();
    ~UART();
    void transmissionUART(uint8_t data);
    uint8_t receive();

};

#endif // UART_H