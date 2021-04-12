#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "debug.h"
#include "Button.h"


void pressButton(){
    enum boutonAppuye {
        one,
        two,
        four,
        r,
        v,
        c,
        i,
        e,
        diez
    };

        boutonAppuye bouton;
        DDRC = 0xE0;
        PORTC |= _BV(PC7) | _BV(PC6) | _BV(PC5);
        DDRA = 0x01;

        if (PINC & 0x04){
            DDRC = 0x1C;
            PORTC = 0x04;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    DEBUG_PRINT(" # ", 4);
                    bouton = diez; // ajouter quoi faire
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    DEBUG_PRINT(" C ", 4);
                    bouton = c;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    DEBUG_PRINT(" 4 ", 4);
                    bouton = four;
                }

        }
        if (PINC & 0x08){
            DDRC = 0x1C;
            PORTC = 0x08;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    DEBUG_PRINT(" E ", 4);
                    PORTA = 0x01; // ajouter quoi faire
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    DEBUG_PRINT(" V ", 4);
                    bouton = v;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    DEBUG_PRINT(" 2 ", 4);
                    bouton = two;
                }

        }
        if (PINC & 0x10){ 
            DDRC = 0x1C;
            PORTC = 0x10;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    DEBUG_PRINT(" I ", 4);
                    PORTA = 0x00; // ajouter quoi faire
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    DEBUG_PRINT(" R ", 4);
                    bouton = r;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    DEBUG_PRINT(" 1 ", 4);
                    bouton = one;
                }
		}

}


int main()
{

	for(;;){
    pressButton();
	}
    return 0;
}
