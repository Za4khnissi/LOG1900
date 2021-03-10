/*
Programme qui change la couleur d'une DEL selon l'intensite de la lumiere recue par la photoresistance.
Broches d'entrees : A0
Broches de sortie : B1 B0
Date : 08/02/2021
Auteur : Zakarya Khnissi - Einstein Franck Tiomo Epongo
Matricules : 1989641 - 2093771
Groupe 01

*/
#define F_CPU 8000000UL
#include "./can.h"
#include <util/delay.h>

const uint8_t DEL_VERTE = 0x01;
const uint8_t DEL_ROUGE = 0x02;
const uint8_t DEL_ETEINTE = 0x00;

const uint8_t VALEUR_DECALAGE = 2;

// ces valeurs ont ete obtenue apres des mesures experimentales faites directement sur le circuit de SimulIDE
const uint8_t INTENSITE_MAX_FAIBLE = 140; 
const uint8_t INTENSITE_MIN_FORTE = 150;
const uint8_t INTENSITE_MAX_FORTE = 255;

void Del_Ambree(){

    PORTB = DEL_VERTE;
    _delay_ms(10);
    PORTB = DEL_ROUGE;
    _delay_ms(10);
}

void Del_Rouge(){

    PORTB = DEL_ROUGE;
}

void Del_Verte(){

    PORTB = DEL_VERTE;
}

bool FaibleIntensite(uint8_t intensite){
    
    if(intensite <= INTENSITE_MAX_FAIBLE){ // intervalle de resistance comprise entre 11 kOhms et 8 kOhms
        return true;
    }
    return false;
}

bool ForteIntensite(uint8_t intensite){

    if (intensite >=  INTENSITE_MIN_FORTE && intensite <= INTENSITE_MAX_FORTE){ // intervalle de resistance comprise entre 4 kOhms et 7 kOhms
        return true;
    }
    return false;
}

int main(){

    can capteur;

    DDRA |= ~(_BV(PA0));
    DDRB |= _BV(PB1) | _BV(PB0); 

    for(;;){
        uint8_t intensite = capteur.lecture(0) >> VALEUR_DECALAGE; // conversion de la valeur analogique a numerique sur la broche A0

        if (FaibleIntensite(intensite)){ 
            Del_Verte();
        }
        else if (ForteIntensite(intensite)){ 
            Del_Rouge();
        }
        else{ // si l'intervalle de resistance comprise entre 7 kOhms et 8 kOhms
            Del_Ambree();
        }
    }
}