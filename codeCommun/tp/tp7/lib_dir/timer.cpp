#include "timer.h"


void timer::startTimer(uint16_t duration, uint16_t start, unsigned int prescaler)
{

    // mode CTC du timer 1 avec horloge divisée par 1024
    TCNT1 = start;
    OCR1A = duration;
    TCCR1A = 1 << WGM12;
    setPrescaler(prescaler); // TCCR1B 
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}


void timer::initializeTimer(uint8_t valueDDRx, uint8_t valueDDRy, volatile uint8_t* DDRx, volatile uint8_t* DDRy) {

    cli ();

    *DDRx = valueDDRx;
    *DDRy = valueDDRy;

    EIMSK |= (1 << INT0);

    EICRA |= (1 << ISC01) | (1 << ISC00);

    sei ();
}


void timer::setPrescaler(unsigned int prescaler)
{
    TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
    switch (prescaler) {

        case 1: // clk/1 (No prescaling)
        TCCR1B |=  (1 << CS10);
        break;

        case 8: // clk/8 (From prescaler)
        TCCR1B |=  (1 << CS11);
        break;

        case 64: // clk/64 (From prescaler)
        TCCR1B |=  (1 << CS11) | (1 << CS10);
        break;

        case 256: // clk/256 (From prescaler)
        TCCR1B |=  (1 << CS12);
        break;

        case 1024: // clk/1024 (From prescaler)
        TCCR1B |=  (1 << CS12) | (1 << CS10);
        break;

        default:
        //printf("Entrer une valeur valide. \n");
        break;
    }
}



