#ifndef CONVERTER_H
#define CONVERTER_H

#include <avr/io.h>
#include <util/delay.h>
#include "Can.h"


enum AnalogDigConv
{
    INTERNAL,
    EXTERNAL
};

void selectCan(AnalogDigConv can)
{
    switch (can)
    {
    case INTERNAL:
        PORTA &= ~(1 << PA0);
        break;

    case EXTERNAL:
        PORTA |= (1 << PA0);
        break;
    }
}


uint8_t computeConversion(AnalogDigConv converter)
{
    selectCan(converter);
    if (converter == AnalogDigConv::INTERNAL)
    {
        Can can;
        uint8_t volts = can.lecture(PA1) >> 2;
        return volts; //* (5/1024);
    }
    else
    {
        uint8_t result = 0;
        PORTB &= ~(1 << PB2) & ~(1 << PB3) & ~(1 << PB4);

        PORTB |= (1 << PB2) | (1 << PB3) | (1 << PB4); //MUX D7
        _delay_ms(10);
        result = PINB & (1 << PB1);       //0000 0001
        PORTB |= (1 << PB3) | (1 << PB4); //MUX D6
        PORTB &= ~(1 << PB2);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 1; //0000 0011

        PORTB |= (1 << PB2) | (1 << PB4); //MUX D5
        PORTB &= ~(1 << PB3);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 2;

        PORTB |= (1 << PB4); //MUX D4
        PORTB &= ~((1 << PB2) | (1 << PB3));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 3;

        PORTB |= (1 << PB2) | (1 << PB3); //MUX D3
        PORTB &= ~(1 << PB4);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 4;

        PORTB |= (1 << PB3); //MUX D2
        PORTB &= ~((1 << PB2) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 5;

        PORTB |= (1 << PB2); //MUX D1
        PORTB &= ~((1 << PB3) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 6;

        PORTB &= ~((1 << PB2) | (1 << PB3) | (1 << PB4)); //MUX D0
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 7;
        return result >> 1;
    }
}

#endif