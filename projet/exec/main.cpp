#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "debug.h"
#include "Button.h"
#include "Can.h"
#include "UART.h"

uint16_t period = 7812;
volatile uint8_t lecture = 0;

    enum boutonAppuye {
        ONE,
        TWO,
        FOUR,
        R,
        V,
        C,
        I,
        E,
        HASHTAG
    };

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

ISR(TIMER1_COMPA_vect)
{
    lecture = 1;
}

// faire ca a partir de librairie ****

void partirMinuterie(uint16_t duree)
{

    // mode CTC du timer 1 avec horloge divisée par 1024
    // interruption après la durée spécifiée
    lecture = 0;

    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A = 1 << WGM12;
    TCCR1B = 1 << CS12 | 1 << CS10;
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}

// faire ca a partir de librairie ****
void initialisation(){
    cli();
    //DDRC |= 0xFC;
    //EIMSK |= (1 << INT0);
    sei();
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

boutonAppuye pressButton(){

        boutonAppuye bouton;
        DDRC = 0xE0;
        PORTC |= _BV(PC7) | _BV(PC6) | _BV(PC5);
        DDRA = 0x01;

        if (PINC & 0x04){
            DDRC = 0x1C;
            PORTC = 0x04;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    
                    bouton = HASHTAG;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    DEBUG_PRINT("Le bouton C du clavier a été appuyé.\n", 41);
                    bouton = C;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    DEBUG_PRINT("Le bouton 4 du clavier a été appuyé.\n", 41);
                    bouton = FOUR;
                }

        }
        if (PINC & 0x08){
            DDRC = 0x1C;
            PORTC = 0x08;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = E;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = V;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = TWO;
                }

        }
        if (PINC & 0x10){ 
            DDRC = 0x1C;
            PORTC = 0x10;
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = I; 
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = R;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = ONE;
                }
		}
    return bouton;
}

void changeFrequency(boutonAppuye bouton){
    switch(bouton){
        case ONE:
            period = 7812;
            break;
        case TWO:
            period = 7812/2;
            break;
        case FOUR:
            period = 7812/4;
            break;
    }
}

uint8_t changeDetectionMode(boutonAppuye bouton, uint8_t var){
    switch(bouton){
        case E:
            var = distance(AnalogDigConv::external);
        case I:
            var = distance(AnalogDigConv::internal);
    }
    return var;
}

int main()
{

    for(;;){
    boutonAppuye bouton = pressButton();
        switch(bouton){
            case ONE:
            DEBUG_PRINT("Le bouton 1 du clavier a été appuyé.\n", 41);
            break;
            case TWO:
            DEBUG_PRINT("Le bouton 2 du clavier a été appuyé.\n", 41);
            break;
            case FOUR:
            DEBUG_PRINT("Le bouton 4 du clavier a été appuyé.\n", 41);
            break;
            case R:
            DEBUG_PRINT("Le bouton R du clavier a été appuyé.\n", 41);
            break;
            case V:
            DEBUG_PRINT("Le bouton V du clavier a été appuyé.\n", 41);
            break;
            case C:
            DEBUG_PRINT("Le bouton C du clavier a été appuyé.\n", 41);
            break;
            case I:
            DEBUG_PRINT("Le bouton I du clavier a été appuyé.\n", 41);
            break;
            case E:
            DEBUG_PRINT("Le bouton E du clavier a été appuyé.\n", 41);
            break;
            case HASHTAG:
            DEBUG_PRINT("Le bouton # du clavier a été appuyé.\n", 41);
            break;

        }
    }
}
