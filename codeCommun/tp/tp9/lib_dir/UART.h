#ifndef UART_H
#define UART_H

#include <avr/io.h>

class UART {
public:
    UART();
    ~UART();
    void transmissionUART(uint8_t data);
    void transmissionMessage(const char message[], uint8_t taille);
    uint8_t receiveUART();

};

#endif // UART_H
