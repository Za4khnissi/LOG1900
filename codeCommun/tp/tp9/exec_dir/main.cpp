#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/eeprom.h>

#include "UART.h"
#include "debug.h"
#include "Utils.cpp"

#define UNE_FOIS 1

//void eeprom_write_byte (uint8_t * adresse, uint8_t value);
//uint8_t eeprom_read_byte (const uint8_t * adresse);


/**
 * fonction qui recoit les instructions de Uart pour l'ecrire sur la memoire
 */
uint16_t receptionInstrustions() {
    UART uart;

    uint8_t* address = 0x00;
    uint16_t instructionSize;

    //Test Del

    //
    uint8_t octet1 = uart.receive();
    
    //on ecrit le premier octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, octet1);
    address++;

    //On recoit maintenant le 2e octet de UART
    uint8_t octet2 = uart.receive();

    //on ecrit le deuxieme octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, octet2);
    address++;

    //le nombre d'instruction sont stocke dans instructionSize
    instructionSize = ((octet1 << 8) | octet2);

    for(uint16_t i = 2; i < instructionSize; i++) {
        //Donnee recue de UART (1byte)
        uint8_t dataFromUART = uart.receive();

        //On ecrit les donnes recues sur la memoire EEPROM
        eeprom_write_byte(address, dataFromUART);

        //On increment l'adresse
        address++;
    }
    return instructionSize;
}



enum Instructions {
    dbt = 0x01, // debut
    att = 0x02, // attendre
    dal = 0x44, // allumer DEL bicolore
    det = 0x45, // eteindre DEL bicolore
    mon = 0x18, // allumer matrice de DEL
    mof = 0x19, // eteindre matrice de DEL
    mar = 0x60, // arreter moteurs
    mbr = 0x61, // arreter moteurs
    mav = 0x62, // avancer
    mre = 0x63, // reculer
    trd = 0x64, // tourner a droite
    trg = 0x65, // tourner gauche
    dbc = 0xA0, // debut boucle
    fbc = 0xA1, // fin boucle
    fin = 0xFF, // fin
};



int main() {

    //Initialisation des entrees/sorties
    DDRA = 0xff; // Port A en mode sortie
    DDRB = 0xff; // Port B en mode sortie
    DDRC = 0xff; // Port C en mode sortie
    DDRD = 0xff; // Port D en mode sortie
    
    //Test Del
    PORTB = 0x01;

    UART uart;

    //Program counter
    uint8_t* compteurProg = 0x00;

    //adresse de la boucle
    uint8_t* adressBoucle;

    //compteur de boucle
    uint8_t compteurBoucle;

    //Variable qui stocke la valeur retourne lors de la lecture de EEPROM
    uint8_t instruction;

    //Variable qui stock l'operande qui suit l'instruction
    uint8_t operande;

    //Ici on fait appel a la fonction receptionInstrustions 
    //qui ecrit les donnees recues par UART sur la memoire
    //et par la suite nous retourne la taille des instructions
    uint16_t instructionSize = receptionInstrustions();

    //instanciation des objets le cas echeants
    

    //variable boolenne retournant le debut de l'execution des instructions
    //pour verifier si l'instruction dbt(debut) est executee avant de rentrer
    //dans le switch statement
    bool executionCode = false;


    for(uint16_t i = 0; i < instructionSize; i++){

        //lecture de l'instruction
        instruction = eeprom_read_byte(compteurProg);
        compteurProg++;

        //Afficheur 7segments
        PORTC = instruction;

        //lecture de l'operande
        operande = eeprom_read_byte(compteurProg);
        compteurProg++;

        //on transmet vers pc l'instruction lue
        uart.transmissionUART(instruction);

        //on verifie si l'instruction est debut avant de rentrer 
        //dans le switch statement dans la premiere fois sinon on fait rien
        if(instruction == dbt)
            executionCode = true;


        if(executionCode){

            switch (instruction) {

                case att:
                    DEBUG_PRINT(("Instruction attendre...\n"));
                    // A completer

                    break;

                case dal:
                    DEBUG_PRINT(("Instruction allumer Del...\n"));
                    // A completer

                    break;

                case det:
                    DEBUG_PRINT(("Instruction eteindre Del...\n"));
                    // A completer

                    break;

                case mon:
                    DEBUG_PRINT(("Instruction allumer matrix Del...\n"));
                    allumerMatrice(operande);
                    break;

                case mof:
                    DEBUG_PRINT(("Instruction eteindre matrix Del...\n"));
                    eteindreMatrix();
                    break;

                case mar:
                    DEBUG_PRINT(("Instruction arreter moteur...\n"));
                    // A completer

                    break;

                case mbr:
                    DEBUG_PRINT(("Instruction arreter moteur...\n"));
                    // A completer

                    break;

                case mav:
                    DEBUG_PRINT(("Instruction avancer moteur...\n"));
                    // A completer

                    eteindreDirection();
                    directionNord();
                    _delay_ms(3000);
                    break;

                case mre:
                    DEBUG_PRINT(("Instruction reculer moteur...\n"));
                    // A completer

                    eteindreDirection();
                    directionSud();
                    _delay_ms(3000);
                    break;

                case trd:
                    DEBUG_PRINT(("Instruction tourner moteur a droite...\n"));
                    // A completer
                    
                    eteindreDirection();
                    directionEst();
                    _delay_ms(3000);
                    break;

                case trg:
                    DEBUG_PRINT(("Instruction tourner moteur a gauche...\n"));
                    // A completer

                    eteindreDirection();
                    directionOuest();
                    break;

                case dbc:
                    DEBUG_PRINT(("Instruction debut boucle...\n"));
                    //compteurProg nous indique l'adresse du debut de la boucle
                    adressBoucle = compteurProg + UNE_FOIS;
                    //on met dans compteurBoucle le nombre d'iteration de la boucle plus une fois
                    compteurBoucle = operande + UNE_FOIS;
                    break;

                case fbc:
                    DEBUG_PRINT(("Instruction fin boucle...\n"));
                    //On verifie si le nombre d'iteration est termine
                    if(compteurBoucle > 0){
                        compteurProg = adressBoucle;
                        compteurBoucle--;
                    }
                    break;

                case fin:
                    DEBUG_PRINT(("Instruction fin tout court...\n"));
                    //A completer si possible sinon un return 0 suffira

                    return 0;
                    break;    

            }
        }   
    }   
    //return 0;
}