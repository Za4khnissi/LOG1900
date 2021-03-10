#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/interrupt.h>

#define ANTIREBOND 10
#define DELAI_BOUTON 200


bool boutonAppuye(uint8_t PINDx) {
   if (PIND & (1 << PINDx)) {
        _delay_ms(ANTIREBOND);
        if (PIND & (1 << PINDx))
            return true;
   }
   return false;
}


void lumiereEteinte(){
    PORTA = 0x00;
}
void lumiereVerte(){
    PORTA = 0x02;
}
void lumiereRouge(){
    PORTA = 0x01;
}


enum EtatMachine {
    INIT,           // Etat initial ou la lumiere est eteinte
    E1,             // d2 --> rouge
    E2a,            // d2 --> vert
    E2b,            // d3 --> lumiere eteinte
};


volatile EtatMachine etat = INIT;



ISR (INT0_vect) {

    // laisser un delai avant de confirmer la réponse du
    // bouton-poussoir: environ 30 ms (anti-rebond)
    _delay_ms (ANTIREBOND);

    // se souvenir ici si le bouton est pressé ou relâché

    // changements d'états tels que ceux de la
    // semaine précédente
    if(PIND & 0x04) {
        switch (etat) {

            case INIT:
                //if(!boutonAppuye(PIND2))
                    etat = E1;
            break;

            case E1:
                //if(!boutonAppuye(PIND2))
                    etat = E2a;
            break;

            case E2a:
                //if(!boutonAppuye(PIND2))
                    etat = INIT;
            break;
    
        }
    }

    // Voir la note plus bas pour comprendre cette instruction et son rôle
    EIFR |= (1 << INTF0) ;
}


void initialisation ( void ) {

    cli ();

    // configurer et choisir les ports pour les entrées
    // et les sorties. DDRx... Initialisez bien vos variables
    DDRA = 0xff; // PORT A est en mode sortie
    DDRD = 0x00; // PORT D est en mode entre

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

    for(;;) {
        switch (etat){

            case INIT:
                lumiereEteinte();
                
                if(boutonAppuye(PIND3)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND3)){
                        etat = INIT;
                    }
                }
            break;


            case E1:
                lumiereRouge();

                if(boutonAppuye(PIND3)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND3)){
                        etat = E2b;
                    }
                }
            break;


            case E2a:
                lumiereVerte();

                if(boutonAppuye(PIND3)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND3)){
                        etat = INIT;
                    }
                }
            
            break;


            case E2b:
                lumiereEteinte();
                
                if(boutonAppuye(PIND3)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND3)){
                        etat = E2a;
                    }
                }

            break;

        }
    }

    return 0;
}