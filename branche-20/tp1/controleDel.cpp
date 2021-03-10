#include <avr/io.h> 

#define F_CPU 8000000
#include <util/delay.h>




int main() {

    DDRA = 0xff; // Port A en mode sortie

    for(;;) {
        PORTA = 0x01;
        _delay_ms (100);
        PORTA = 0x02;
        _delay_ms(100);
    }

    return 0;
}