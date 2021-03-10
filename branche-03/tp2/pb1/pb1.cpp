/*
Machine à états de Moore contrôlant l'allumage d'une LED
Date : 08/02/2021
Auteur : Zakarya Khnissi - Einstein Franck Tiomo Epongo
Matricules : 1989641 - 2093771
Groupe 01
        --------------------------------------------------------------------------------------
        |   État présent    |    Entrée     | État suivant |            Sortie               |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E0       | La lampe prend la couleur verte |
        |       INIT        |----------------------------------------------------------------|
        |                   | Appuie sur D3 |    INIT      | La lampe reste eteinte          |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E0       | La lampe reste verte            |
        |       E0          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     E1       | La lampe reste verte            |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E1       | La lampe reste verte            |
        |       E1          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     E2       | La lampe reste verte            |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E2       | La lampe reste verte            |
        |       E2          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     E3       | La lampe est rouge pendant 1s   |
        |------------------------------------------------------------------------------------|
        |                   | temps < 1s    |     E3       | La lampe reste rouge            |
        |       E3          |----------------------------------------------------------------|
        |                   | temps >= 1s   |    INIT      | La lampe s'éteint               |
        |------------------------------------------------------------------------------------|
*/


#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



bool presser_relacher(uint8_t bouton) 
{
    bool estRelache = false;
    if(PIND & bouton)
    {
        while(PIND & bouton) {}
        estRelache =  true;
    }
    return estRelache;
}


void afficher(uint8_t del)
{
    PORTA = del; 
}


int main ()
{
    const uint8_t D2 = 0x04;
    const uint8_t D3 = 0x08;

    const uint8_t DEL_eteinte = 0x00;
    const uint8_t DEL_verte = 0x01;
    const uint8_t DEL_rouge = 0x02;

    DDRD = 0x00;
    DDRA = 0xff;

    enum Etat { INIT, E0, E1, E2, E3 };

    Etat etat_actuel = Etat::INIT;
    
    for (;;)
    {
        switch (etat_actuel)
        {
            case INIT:            
                if (presser_relacher(D2)) // Le bouton-possoir D2 est pressé et relâché
                {   
                    afficher(DEL_verte);
                    etat_actuel = E0;
                }
                break;

            case E0: 
            case E1:
            case E2:
                if (presser_relacher(D3))
                {   
                    etat_actuel = (Etat)(etat_actuel + 1); // Passer au prochain état
                }
                break;

            case E3:
                afficher(DEL_rouge);
                _delay_ms(1000);
                afficher(DEL_eteinte);
                etat_actuel = INIT;
                break;
        }
    }
    return 0;
}