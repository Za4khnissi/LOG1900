/*
Machine à états de Moore contrôlant l'allumage d'une LED
Date : 08/02/2021
Auteur : Zakarya Khnissi - Einstein Franck Tiomo Epongo
Matricules : 1989641 - 2093771
Groupe 01
        --------------------------------------------------------------------------------------
        |   État présent    |    Entrée     | État suivant |            Sortie               |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E0       | La lampe prend la couleur rouge |
        |       INIT        |----------------------------------------------------------------|
        |                   | Appuie sur D3 |    INIT      | La lampe reste eteinte          |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E2       | La lampe prend la couleur verte |
        |       E0          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     E1       | La lampe s'éteint               |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     E1       | La lampe reste eteinte          |
        |       E1          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     E2       | La lampe prend la couleur verte |
        |------------------------------------------------------------------------------------|
        |                   | Appuie sur D2 |     INIT     | La lampe s'éteint               |
        |       E2          |----------------------------------------------------------------|
        |                   | Appuie sur D3 |     INIT     | La lampe s'éteint               |
        |------------------------------------------------------------------------------------|
*/

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t etat = 0;

// placer le bon type de signal d'interruption

// à prendre en charge en argument

ISR ( 'modifier ici' ) {

// laisser un delai avant de confirmer la réponse du

// bouton-poussoir: environ 30 ms (anti-rebond)

_delay_ms ( 30 );

// se souvenir ici si le bouton est pressé ou relâché

'modifier ici'

// changements d'états tels que ceux de la

// semaine précédente

'modifier ici'

// Voir la note plus bas pour comprendre cette instruction et son rôle

EIFR |= (1 << INTF0) ;

}


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


int main()
{
    const uint8_t D2 = 0x04;
    const uint8_t D3 = 0x08;

    const uint8_t DEL_eteinte = 0x00;
    const uint8_t DEL_verte = 0x01;
    const uint8_t DEL_rouge = 0x02;

    DDRD = 0x00;
    DDRA = 0xff;
    
    enum Etat {INIT, E0, E1, E2};

    Etat etat_actuel = INIT;
    
    for (;;)
    {
        switch (etat_actuel)
        {
            case INIT :
                if (presser_relacher(D2))
                {
                    afficher(DEL_rouge);
                    etat_actuel = E0; 
                }
                break;

            case E0 :
                if (presser_relacher(D2))
                {
                    afficher(DEL_verte); 
                    etat_actuel = E2;
                }
                else if (presser_relacher(D3))
                {
                    afficher(DEL_eteinte); 
                    etat_actuel = E1;
                }
                break;

            case E1 :
                if (presser_relacher(D3))
                {
                    afficher(DEL_verte);
                    etat_actuel = E2;
                }
                break;

            case E2 : 
                if (presser_relacher(D2) || (presser_relacher(D3)))
                {
                    afficher(DEL_eteinte);
                    etat_actuel = INIT;
                }
                break;
        }
    }
    return 0;
}