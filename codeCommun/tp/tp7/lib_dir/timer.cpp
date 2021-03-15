#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include "timer.h"
using namespace timer

void timer::partirMinuterie(uint16_t duree, uint16_t debut)
{

    minuterieExpiree = 0;

    // mode CTC du timer 1 avec horloge divisée par 1024

    // interruption après la durée spécifiée

    TCNT1 = debut;
    OCR1A = duree;
    TCCR1A = 1 << WGM12;
    TCCR1B = 1 << CS12 | 1 << CS10;
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}



void timer::initialisationMinuterie (uint8_ valeurDDRA, uint8_t valeurDDRD) {

// cli est une routine qui bloque toutes les interruptions.

// Il serait bien mauvais d'être interrompu alors que

// le microcontroleur n'est pas prêt...

cli ();


// configurer et choisir les ports pour les entrées

// et les sorties. DDRx... Initialisez bien vos variables

DDRA = sortie;
DDRD = entree;


// cette procédure ajuste le registre EIMSK

// de l’ATmega324PA pour permettre les interruptions externes

EIMSK |= (1 << INT0) ;


// il faut sensibiliser les interruptions externes aux

// changements de niveau du bouton-poussoir

// en ajustant le registre EICRA

EICRA |= (1 << ISC01) | (1 << ISC00);


// sei permet de recevoir à nouveau des interruptions.

sei ();
}



