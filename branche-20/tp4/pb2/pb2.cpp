#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define ANTIREBOND 10

volatile bool minuterieExpiree;
volatile bool boutonPoussoir; 

void lumiereEteinte(){
    PORTA = 0x00;
}
void lumiereVerte(){
    PORTA = 0x02;
}
void lumiereRouge(){
    PORTA = 0x01;
}

enum Etat {
    INIT,
    REUSSI,
    NON_REUSSI
};

volatile Etat etat = INIT;

ISR (TIMER1_COMPA_vect) {
    minuterieExpiree = 1;
}


ISR (INT0_vect) {
    // anti-rebond
    _delay_ms(ANTIREBOND);

    if(PIND & 0X04) {
        boutonPoussoir = 1;
        etat = REUSSI;
    }
}


void partirMinuterie (uint16_t duree) {
    minuterieExpiree = 0;

    // mode CTC du timer 1 avec horloge divisée par 1024
    // interruption après la durée spécifiée
    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A |= (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A);
}


void initialisation ( void ) {
    cli ();

    // configurer et choisir les ports pour les entrées
    // et les sorties. DDRx... Initialisez bien vos variables
    DDRA = 0xff; // PORT A est en mode sortie: 0b 1111 1111
    DDRD = 0x20; // PORT D est en mode entre:  0b 0010 0000

    // cette procédure ajuste le registre EIMSK
    // de l’ATmega324PA pour permettre les interruptions externes
    EIMSK |= (1 << INT0) ;

    // il faut sensibiliser les interruptions externes aux
    // changements de niveau du bouton-poussoir
    // en ajustant le registre EICRA
    EICRA |= (1 << ISC01) | (1 << ISC00) ;

    // sei permet de recevoir à nouveau des interruptions.
    sei ();
}

int main() {

    initialisation();
    _delay_ms(3000);
    lumiereRouge();
    _delay_ms(100);
    lumiereEteinte();

    do {
    // attendre qu'une des deux variables soit modifiée
    // par une ou l'autre des interruptions.
    } while ( minuterieExpiree == 0 && boutonPoussoir == 0 );


    // Une interruption s'est produite. Arrêter toute
    // forme d'interruption. Une seule réponse suffit.
    cli ();

    switch (etat)
    {
        case INIT:
            if(boutonPoussoir == 1)
                etat = REUSSI;
            else
                etat = NON_REUSSI;
            break;

        case REUSSI:
            lumiereVerte();
        break;

        case NON_REUSSI:
            lumiereRouge();
        break;

    }

    return 0;
}