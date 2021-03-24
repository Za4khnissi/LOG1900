/*************************************************************************************************************************************
Fichier : pb1.cpp
TP2_INF1900
Auteur : Zabiullah SZ & Moussa Daoud
Date : 31 Jan 2021
Description du TP : Machines à états finis logicielles
Description du probleme :
On appuie le bouton-poussoir relie a D2 pour que la DEL tourne au vert.
Par la suite, on appuie 3 fois sur le bouton-poussoir sur D3 pour que la DEL prenne la couleur rouge pendant exactement 1 seconde.
finalement le systeme revient a l'etat initial
*************************************************************************************************************************************



Tableau des états
---------------------------------------------------------------
Etat present    Entree D2  Entree D3   Etat suivant       Sortie       
    INIT             x         0            INIT              0
    INIT             0         1             E1a              0
    E1a              0         x             E1a              0
    E1a              1         x             E1b              0
    E1b              0         x             E1b              0
    E1b              1         x             E2a              0
    E2a              0         x             E2a              0
    E2a              1         x             E2b              0
    E2b              0         x             E2b              0
    E2b              1         x             E3a              0
    E3a              0         x             E3a              0
    E3a              1         x             E3b              0
    E3b              0         x             E3b              0
    E3b              1         x             E4               0
    E4               0         x             INIT             1 

    
    
*/

#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include "debug.h"


#define ANTIREBOND 10
#define UNE_SECONDE 1000


/*************************************************************************************
  * Fonction:	boutonAppuye
  * Description: Méthode (fonction antirebond) qui verifie si le PINDx est presse
  * Paramètres:	- le numero de la PIN
  * Retour:		un booleen qui represente si le bouton est appuyé d'une facon naturelle
  ***********************************************************************************/
bool boutonAppuye(uint8_t PINDx) {
    if (PIND & (1 << PINDx)) {
        _delay_ms(ANTIREBOND);
        if (PIND & (1 << PINDx))
            return true;
    }
    return false;
}



/*************************************************************************************
  * Fonction:	lumiereEteinte
  * Description: Méthode qui met le port a 0 pour eteindre la lumiere 
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereEteinte(){
    PORTA = 0x00;
}

/*************************************************************************************
  * Fonction:	lumiereVerte
  * Description: Méthode qui allume la lumiere verte
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereVerte(){
    PORTA = 0x02;
}

/*************************************************************************************
  * Fonction:	lumiereRouge
  * Description: Méthode qui allume la lumiere rouge
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereRouge(){
    PORTA = 0x01;
    _delay_ms(UNE_SECONDE);
    lumiereEteinte();
}

enum EtatMachine {
    INIT,// E0 ou la lumiere est eteinte
    E1a, // Etat ou la lumiere est verte (D2 presse)
    E1b, // Etat ou la lumiere est tjr verte et D3 est presse une 1ere fois
    E2a, // D3 est relache
    E2b, // D3 est presse une 2e fois
    E3a, // D3 est relache
    E3b, // D3 est presse une 3e fois
    E4   // Etat final ou la rouge est allumee pdt une seconde (D3 presse 3fois)
};

void initialiser()
{
    DDRA |= _BV(PA0) | _BV(PA1); // PA0 et PA1 en mode sortie
    DDRD &= ~( _BV(PD2) | _BV(PD3) ); // PD2 et PD3 en mode entre
    INIT_DEBUG;
}

void transmissionMessage(const char etatActuel[], uint8_t taille)
{
    char messageInitial[14] = "Etat actuel: ";
    
    for ( uint8_t i = 0; i < 13; i++ ) {
        transmissionUART( messageInitial[i] );
    }

    for ( uint8_t i = 0; i < taille ; i++ ) {
        transmissionUART( etatActuel[i] );
    }
}

int main() {

    initialiser();

    EtatMachine etat = INIT;

    for(;;) {

        switch (etat){

            case INIT:
                DEBUG_PRINT("INIT\n", 6);
                lumiereEteinte();
                if(boutonAppuye(PIND2)){
                    lumiereVerte();
                    etat = E1a;
                }
                break;

            case E1a:
                DEBUG_PRINT("E1a\n", 5);
                if(boutonAppuye(PIND3))
                    etat = E1b;
                break;

            case E1b:
                DEBUG_PRINT("E1b\n", 5);
                if(!boutonAppuye(PIND3))
                    etat = E2a;
                break;

            case E2a:
                DEBUG_PRINT("E2a\n", 5);
                if(boutonAppuye(PIND3))
                    etat = E2b;
                break;

            case E2b:
                DEBUG_PRINT("E2b\n", 5);
                if(!boutonAppuye(PIND3))
                    etat = E3a;
                break;

            case E3a:
                DEBUG_PRINT("E3a\n", 5);
                if(boutonAppuye(PIND3))
                    etat = E3b;
                break;

            case E3b:
                DEBUG_PRINT("E3b\n", 5);
                if(!boutonAppuye(PIND3))
                    etat = E4;
                break;

            case E4:
                DEBUG_PRINT("E4\n", 4);
                lumiereEteinte();
                lumiereRouge();
                etat = INIT;
                break;
        }
    }

    return 0;
}