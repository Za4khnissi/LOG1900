#include "UARTtp8.h"

void UART::initializeUART ( void ) {

    // 2400 bauds. Nous vous donnons la valeur des deux
    // premier registres pour vous éviter des complications
    UBRR0H = 0;
    UBRR0L = 0xCF;

    // permettre la réception et la transmission par le UART0
    UCSR0A = 0 ;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) ;

    // Format des trames: 8 bits, 1 stop bits, none parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0) | (0 << UPM01) | (0 << UPM00)  ;
}

void UART::transmissionUART ( uint8_t data ) {

    while ( !( UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}


