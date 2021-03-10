/*************************************************************************************************************************************
Fichier : pb1.cpp
TP6_INF1900
Auteur : Zabiullah SZ & Moussa Daoud
Date : 06 Mars 2021
Description du TP : Capteurs et conversion analogique/numérique
Description du probleme :
Faire un robot pouvant se faire bronzer...
Si la lumière est basse sur la photorésistance, la DEL prendra la couleur verte.
Si la lumière est à un bon niveau (lumière ambiante), la DEL tournera à l'ambré.
Si la lumière est trop forte (photorésistance sous une lampe de poche), la DEL devient rouge.
*************************************************************************************************************************************/

#include <avr/io.h>
#include "can.h"


/*************************************************************************************
  * Fonction:	lumiereVerte
  * Description: Méthode qui allume la lumiere verte
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereVerte(){
    PORTB = 0x01;
}

/*************************************************************************************
  * Fonction:	lumiereRouge
  * Description: Méthode qui allume la lumiere rouge
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereRouge(){
    PORTB = 0x02;
}


/*************************************************************************************
  * Fonction:	lumiereAmbre
  * Description: Méthode qui allume la lumiere ambre
  * Paramètres:	- rien
  * Retour:		rien
  ***********************************************************************************/
void lumiereAmbre(){
    // valeur d'initialisation de la lumiere ambre
    static bool val = false;
    if(val)
        lumiereRouge();
    else 
        lumiereVerte();
    val = !val;
}



/* Valeur arbitraire
resistance de la photoresistance pour:
                                        une lampe de poche : 4 a 5kΩ
                                        une lumiere ambiante : 5 a 7kΩ
                                        une faible lumiere : 7 a 11kΩ

Valeur de la tension prise au point de contact entre la photoresistance et la resistance de 10kΩ placees en serie 
selon l'intention du courant:
       
        une faible lumiere   : 2.38V a 2.94V      ===========>  [10/11+10)]*5V = 2.38V , [10/(7+10)]*5V = 2.94V
        une lumiere ambiante : 2.94V a 3.33V      ===========>  [10/ 7+10)]*5V = 2.94V , [10/(5+10)]*5V = 3.33V
        une lampe de poche   : 3.33V a 3.57V      ===========>  [10/ 5+10)]*5V = 3.33V , [10/(4+10)]*5V = 3.57V


*/

/* Par la suite le CAN convertira ces valeurs analogiques en des valeurs numeriques en considerant Aref de 5V. 
   Donc ceci donne pour: 
                    une lampe de poche : > (3.33/5)*255 = 170
                    une lumiere ambiante : entre 150 a 170    ((2.94/5)*255 = 150) 
                    une faible lumiere : < 150

*/

int main() {

    DDRA = 0x00; // PORT A est en mode entree
    DDRB = 0xff; // PORT B est en mode sortie

    //instanciation d'un objet de la classe CAN
    can analogDigConv;
    

    //Valeur numerique des intensites apres la conversion
    const uint8_t INTENSITE_LUMIERE_ROUGE = 170;  
    const uint8_t INTENSITE_LUMIERE_VERTE = 150; 

    //Variable qui stocke la valeur de retour de la methode lecture de la classe can
    uint8_t intensiteLumiere = 0;

    //Boucle infinie
    for(;;){
        intensiteLumiere = (analogDigConv.lecture(0) >> 2);

        if(intensiteLumiere <= INTENSITE_LUMIERE_VERTE){
            lumiereVerte();
        }
        else if(intensiteLumiere < INTENSITE_LUMIERE_ROUGE){
            lumiereAmbre();
        }
        else 
            lumiereRouge();
    }

    return 0;
}