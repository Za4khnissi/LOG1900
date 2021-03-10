#include <avr/io.h> 

#define F_CPU 8000000
#include <util/delay.h>


bool debouce() {
    if(PIND & 0x04) {
        _delay_ms(10);
        if(PIND & 0x04)
            return true;
        else 
            return false;
    }
    return false;
}


int main() {

    DDRA = 0XFF;    // Port A en mode sortie
    DDRD = 0X00;    // Port D en mode entre

    for(;;) {
        PORTA = 0x00;
        while(debouce()) {
            PORTA = 0x01;
        }
    }

    return 0;
}