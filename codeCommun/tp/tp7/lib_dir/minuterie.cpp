#include "minuterie.h"


void timer::partirMinuterie(uint16_t duree, uint16_t debut)
{

    // mode CTC du timer 1 avec horloge divisée par 1024
    TCNT1 = debut;
    OCR1A = duree;
    TCCR1A = 1 << WGM12;
    TCCR1B = 1 << CS12 | 1 << CS10;
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}



void timer::initialisationMinuterie (uint8_t valeurDDRx, uint8_t valeurDDRy, volatile uint8_t* DDRx, volatile uint8_t* DDRy) {


cli ();

*DDRx = valeurDDRx;
*DDRy = valeurDDRy;

EIMSK |= (1 << INT0) ;

EICRA |= (1 << ISC01) | (1 << ISC00);

sei ();
}


