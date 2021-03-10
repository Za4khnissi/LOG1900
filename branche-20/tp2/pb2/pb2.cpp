/*************************************************************************************************************************************
Fichier : pb2.cpp
TP2_INF1900
Auteur : Zabiullah Shair Zaie & Moussa Daoud
Date : 31 Jan 2021
Description du TP : Machines à états finis logicielles
**************************************************************************************************************************************

Tableau des etats:*/


////////////////////////////////////////////////////////////////
//État présent   Entrée D2  Entrée D3   État suivant     Sortie D
//INIT             0           0            INIT         Éteint
//INIT             0           1            INIT         Éteint
//INIT             1           0            E1           Éteint
//E1               0           0            E1           Rouge
//E1               1           0            E2a          Rouge
//E1               0           1            E2b          Rouge
//E2a              1           0            INIT         Vert
//E2a              0           1            INIT         Vert
//E2b              1           0            E2b          Éteint
//E2b              0           0            E2b          Éteint
//E2b              0           1            E2a          Éteint
///////////////////////////////////////////////////////////////





#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>


#define ANTIREBOND 10
#define DELAI_BOUTON 200

/*************************************************************************************
  * Fonction:	boutonAppuye
  * Description: Méthode (fonction antirebond) qui verifie si le button PIN est sortie 
  * Paramètres:	- le numero de la PIN
  * Retour:		un booleen qui represente si le PIN est appuyé
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
}

enum EtatMachine {
    INIT,           // Etat initial ou la lumiere est eteinte
    E1,             // d2 --> rouge
    E2a,            // d2 --> vert
    E2b,            // d3 --> lumiere eteinte
};

int main() {

    DDRA = 0xff; // PORT A est en mode sortie
    DDRD = 0x00; // PORT D est en mode entre

    EtatMachine etat = INIT;


    for(;;) {
        switch (etat){

            case INIT:
                lumiereEteinte();
                
                if(boutonAppuye(PIND2)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND2)){
                        etat = E1;
                    }
                }
                else if(boutonAppuye(PIND3)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND3)){
                        etat = INIT;
                    }
                }
            break;


            case E1:
                lumiereRouge();

                if(boutonAppuye(PIND2)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND2)){
                        etat = E2a;
                    }
                }

                    else if(boutonAppuye(PIND3)){
                        _delay_ms(DELAI_BOUTON);
                        if(!boutonAppuye(PIND3)){
                            etat = E2b;
                        }
                    }
            break;


            case E2a:
                lumiereVerte();
                if(boutonAppuye(PIND2)){
                    _delay_ms(DELAI_BOUTON);
                    if(!boutonAppuye(PIND2)){
                        etat = INIT;
                    }
                }

                else if(boutonAppuye(PIND3)){
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