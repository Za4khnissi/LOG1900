#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define DEUX_SECONDE 2000


void ajustementPWM (uint8_t frequence) {
    // mise à un des sorties OC1A et OC1B sur comparaison
    // réussie en mode PWM 8 bits, phase correcte
    // et valeur de TOP fixe à 0xFF (mode #1 de la table 16-5
    // page 130 de la description technique du ATmega324PA)
    OCR1A = frequence;
    OCR1B = frequence;


    // division d'horloge par 8 - implique une frequence de PWM fixe
    TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    TCCR1B |= (1 << CS11);
    TCCR1C = 0;
}


void initialisation ( void ) {
    cli ();
    // configurer et choisir les ports pour les entrées
    // et les sorties. DDRx... Initialisez bien vos variables
    DDRC = 0x00; // PORT A est en mode entre
    DDRD = 0xff; // PORT D est en mode sorti

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


int main(void) {

    initialisation();

    for(;;) {
        ajustementPWM(255); //pwm a 100%
        _delay_ms(DEUX_SECONDE);
        ajustementPWM(191); //pwm a 75%    
        _delay_ms(DEUX_SECONDE);
        ajustementPWM(128); //pwm a 50%
        _delay_ms(DEUX_SECONDE);
        ajustementPWM(64); //pwm a 25%
        _delay_ms(DEUX_SECONDE);
        ajustementPWM(0); //pwm a 0%
        _delay_ms(DEUX_SECONDE);
    }
    return 0;
}