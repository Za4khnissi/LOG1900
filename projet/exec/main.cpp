#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#include "debug.h"
#include "Button.h"

#include <stdio.h>

#include "Can.h"
#include "UART.h"


enum Sensor {
    left,
    center,
    right
};

enum AnalogDigConv {
    internal,
    external
};

void selectSensor(Sensor sensor) {
    switch (sensor){
        case left:
            PORTA &= ~((1 << PA2) | (1 << PA3));
        break;
        
        case center:
            PORTA |= (1 << PA2);
            PORTA &= ~(1 << PA3);
        break;
        
        case right:
            PORTA |= (1 << PA3);
            PORTA &= ~(1 << PA2);
        break;
    }
}

void selectCan(AnalogDigConv can){
    switch (can){
        case internal:
            PORTA &= ~(1 << PA0);
        break;
        
        case external:
            PORTA |= (1 << PA0);
        break;
    }
}

uint8_t distance(AnalogDigConv converter) {
    selectCan(converter);

    if(converter == AnalogDigConv::internal){
        Can can; 
        uint8_t volts = can.lecture(PA1);
        //return can.lecture(PA1) >> 2;
        return volts; //* (5/1024);
    }
    else{
        uint8_t result = 0;

        PORTB &= ~(1 << PB2) & ~(1 << PB3) & ~(1 << PB4);
    
        PORTB |= (1 << PB2) | (1 << PB3) | (1 << PB4); //MUX D7
        _delay_ms(10);
        result = PINB & (1<< PB1);          //0000 0001

        PORTB |= (1 << PB3) | (1 << PB4); //MUX D6
        PORTB &= ~(1 << PB2);
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 1;          //0000 0011
        
        PORTB |= (1 << PB2) | (1 << PB4); //MUX D5
        PORTB &= ~(1 << PB3);
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 2;
        
        PORTB |= (1 << PB4); //MUX D4
        PORTB &= ~((1 << PB2) | (1 << PB3));
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 3;
        
        PORTB |= (1 << PB2) | (1 << PB3); //MUX D3
        PORTB &= ~(1 << PB4);
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 4;
        
        PORTB |= (1 << PB3); //MUX D2
        PORTB &= ~((1 << PB2) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 5;
        
        PORTB |= (1 << PB2); //MUX D1
        PORTB &= ~((1 << PB3) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 6;
        
        PORTB &= ~((1 << PB2) | (1 << PB3) | (1 << PB4)); //MUX D0
        _delay_ms(10);
        result += (PINB & (1<< PB1)) << 7;
        return result;
    }
}

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
                    PORTA = 0x00; 
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
