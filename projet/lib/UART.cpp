#include "UART.h"

UART::UART() {
  // 2400 bauds. Nous vous donnons la valeur des deux
  // premier registres pour vous éviter des complications
    UBRR0H = 0;
    UBRR0L = 0x33;

  // permettre la reception et la transmission par le UART0
    UCSR0A = 0;
    UCSR0B |= (1 << RXEN0 | 1 << TXEN0);  // active la transmission et la reception

  // Format des trames: 8 bits, 1 stop bits, none parity
    UCSR0C = 1 << UCSZ01 | 1 << UCSZ00;
}

UART::~UART(){
    // fais rien de particulier
}

//methode qui envoie les donnees par le port serie de RS232
void UART::transmissionUART(uint8_t data) {
    while (!( UCSR0A & (1<<UDRE0))){
    }
    UDR0 = data;
}

//methode qui retourne les donnees reçues
uint8_t UART::receiveUART() {
    /* Wait for data to be received */
    while (!(UCSR0A & (1<<RXC0))){ 
    }
    return UDR0;
}

void UART::transmissionMessage(const char message[], uint8_t taille)
{
    for ( uint8_t i = 0; i < taille ; i++ ) {
        transmissionUART( message[i] );
    }
}
