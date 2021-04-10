#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>


/*enum Clavier
{
    C1, 
    C2, 
    C3,
    R1, 
    R2,
    R3
};


int colonneActive;
int rangeeActive;


void detectionRangee() {
    DDRC = 0x1C;
    PORTC = 0x1C;
    if (PINC & 0x20)
        rangeeActive = R1;
    if (PINC & 0x40)
        rangeeActive = R2;
    if (PINC & 0x80)
        rangeeActive = R3;

}


void detectionDeBouton() {
    DDRC = 0xE0;

    PORTC |= 0xE0;

    if (PINC & 0x4){
        colonneActive = C3;
        detectionRangee();
    }

    if (PINC & 0x8){
        colonneActive = C2;
        detectionRangee();
    }

    if (PINC & 0x16){
        colonneActive = C1;
        detectionRangee();
    }
}
*/




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
			PORTC &= 0x04;
				if (PINC & 0x80){
					bouton = diez; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = c;
				}
				else if (PINC & 0x20){
					bouton = four;
				}
	
		}
		if (PINC & 0x08){
			DDRC = 0x1C;
			PORTC &= 0x08;
				if (PINC & 0x80){
					PORTA = 0x01; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = v;
				}
				else if (PINC & 0x20){
					bouton = two;
				}
	
		}
		if (PINC & 0x10){
			DDRC = 0x1C;
			PORTC &= 0x10;
				if (PINC & 0x80){
					PORTA = 0x00; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = r;
				}
				else if (PINC & 0x20){
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
