

#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>


void initialisationUART ( void ) {

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

// De l'USART vers le PC

void transmissionUART ( uint8_t donnee ) {

    while ( !( UCSR0A & (1<<UDRE0)) ){
    }
    UDR0 = donnee;
}

int main()
{

    char mots[35] = "Le simulateur SimulIDE en INF1900\n";
    uint8_t i, j;
    for ( i = 0; i < 100; i++ ) {
        for ( j=0; j < 34; j++ ) {
            transmissionUART ( mots[j] );
        }
    }
    return 0;
}


