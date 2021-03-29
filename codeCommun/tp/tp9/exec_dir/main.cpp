#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/eeprom.h>

#include "UART.h"
#include "debug.h"
#include "Motor.h"
#include "Led.h"
#include "Utils.h"

#define UNE_FOIS 1

enum Instruction {

    DBT = 0X01,
    ATT = 0X2,
    DAL = 0X44,
    DET = 0x45,
    MON = 0x18,
    MOF = 0x19,
    MAR = 0x60, 
    MBR = 0x61,
    MAV = 0x62,
    MRE = 0x63,
    TRD = 0x64,
    TRG = 0x65,
    DBC = 0xC0,
    FBC = 0xC1,
    FIN = 0xFF,

};

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

//Instruction: dal
void allumerLed(Led led, uint8_t operande){
    if(operande <= 127){
        led.setLedColor(&PORTB, GREEN);
    }
    else {
        led.setLedColor(&PORTB, RED);

    }
}

//Instruction: det
void eteindreLed(Led led){
    led.setLedColor(&PORTB, OFF);
}

//Instruction: att
void attendre(uint8_t operande) {
    _delay_ms(25*operande);
}

int main() {

    //Initialisation des entrees/sorties
    DDRA = 0xff; // Port A en mode sortie
    DDRB = 0xff; // Port B en mode sortie
    DDRC = 0xff; // Port C en mode sortie
    DDRD = 0xff; // Port D en mode sortie

    //Test Del
    Led led;
    //led.setLedColor(&PORTB, GREEN);
    //PORTC = 0xaa;

    Motor motor;
    UART uart;

    //Program counter
    uint8_t* compteurProg = 0x00;

    //adresse de la boucle
    uint8_t* adressBoucle;

    //compteur de boucle
    uint8_t compteurBoucle;

    //Variable qui stocke la valeur retourne lors de la lecture de EEPROM
    uint8_t instruction = 0;

    //Variable qui stock l'operande qui suit l'instruction
    uint8_t operande = 0;

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
        PORTC = instruction; //---> affichage de l instruction courante
        _delay_ms(1000);
        //lecture de l'operande
        operande = eeprom_read_byte(compteurProg);
        compteurProg++;

        //on transmet vers pc l'instruction lue
        uart.transmissionUART(instruction);

        //on verifie si l'instruction est debut avant de rentrer 
        //dans le switch statement dans la premiere fois sinon on fait rien
        if(instruction == DBT)
            executionCode = true;


        if(executionCode){

            switch (instruction) {

                case ATT:
                    DEBUG_PRINT(("Instruction attendre...\n"));
                    attendre(operande);
                    // A completer

                    break;

                case DAL:
                    DEBUG_PRINT(("Instruction allumer Del...\n"));
                    allumerLed(led, operande);
                    break;

                case DET:
                    DEBUG_PRINT(("Instruction eteindre Del...\n"));
                    // A completer
                    eteindreLed(led);

                    break;

                case MON:
                    DEBUG_PRINT(("Instruction allumer matrix Del...\n"));
                    Utils::allumerMatrice(operande);
                    break;

                case MOF:
                    DEBUG_PRINT(("Instruction eteindre matrix Del...\n"));
                    Utils::eteindreMatrix();
                    break;

                case MAR:
                case MBR:
                    DEBUG_PRINT(("Instruction arreter moteur...\n"));
                    // A completer
                    if(operande == 0x60 || operande == 0x61) {

                        motor.stop();
                    }
                    break;

                case MAV:
                    DEBUG_PRINT(("Instruction avancer moteur...\n"));
                    // A completer
                    motor.forward(operande, operande);

                    Utils::eteindreDirection();
                    Utils::directionNord();
                    break;

                case MRE:
                    DEBUG_PRINT(("Instruction reculer moteur...\n"));
                    // A completer
                    motor.backward(operande, operande);

                    Utils::eteindreDirection();
                    Utils::directionSud();
                    break;

                case TRD:
                    DEBUG_PRINT(("Instruction tourner moteur a droite...\n"));
                    // A completer
                    motor.turnRight();
                    
                    Utils::eteindreDirection();
                    Utils::directionEst();
                    break;

                case TRG:
                    DEBUG_PRINT(("Instruction tourner moteur a gauche...\n"));
                    // A completer
                    motor.turnLeft();

                    Utils::eteindreDirection();
                    Utils::directionOuest();
                    break;

                case DBC:
                    DEBUG_PRINT(("Instruction debut boucle...\n"));
                    //compteurProg nous indique l'adresse du debut de la boucle
                    adressBoucle = compteurProg; //+ UNE_FOIS;
                    //on met dans compteurBoucle le nombre d'iteration de la boucle plus une fois
                    compteurBoucle = operande + UNE_FOIS;
                    break;

                case FBC:
                    DEBUG_PRINT(("Instruction fin boucle...\n"));
                    //On verifie si le nombre d'iteration est termine
                    if(compteurBoucle > 0) {
                        compteurProg = adressBoucle; // Revenir a DBC
                        compteurBoucle--;
                    }
                    break;

                case FIN:
                    DEBUG_PRINT(("Instruction fin tout court...\n"));
                    //A completer si possible sinon un return 0 suffira
                    motor.stop();
                    Utils::eteindreDirection();
                    Utils::eteindreMatrix();
                    _delay_ms(2000);
                    Utils::eteindreAfficheur();
                    eteindreLed(led);
                    return 0;
                    break;    

            }
        }   
    }   
    //return 0;
}


/*void writeEEPROM(uint8_t *adress, uint8_t data){
    data = 0x00;
    do{
        data = receiveUART();
        eeprom_write_byte(adress, data);
        adress++;
    }while(data != FIN)
}

void readEEPROM (uint8_t* adress, uint8_t operand){
    adress = 0x00; // initialisation de l'adresse pour commencer la lecture
    while (eeprom_read_byte(adress) != DBT){ // boucle pour ignorer l'exception et pointer vers DBT
        adress++; // apres la sortie du while adress pointe vers l'instruction 
    }
    while (eeprom_read_byte(adress) != FIN){ // debut de la lecture 
        if (eeprom_read_byte(adress) == DBC){ // si l'instruction est un debut de boucle DBC

            DEBUG_PRINT("Instruction: DBC\n", 17);
            operands = eeprom_read_byte(adress + 1); // initialisation de la boucle
            adress += 0x02; 
            uint8_t counter = 0; // compteur pour verifier si le programme a executer toute les instructions entre DBC et FBC
                do{
                    if (eeprom_read_byte(adress) == FBC){ // 
                        DEBUG_PRINT("Instruction: FBC\n", 17);
                        do {
                            adresse--; // retourner a l'adress de DBC
                        }while(eeprom_read_byte(adress) != DBC);
                        adress += 0x02; 
                        counter++;                                              
                    }                                                           
                    readInstructions(adress);                                   
                    adresse += 0x02;                                            

                } while (counter != (operands + 1) ); 
            }
            readInstructions(adress);
            adress += 0x02;
        }
    }
    readInstruction(adress); // lire l'instruction FIN
}
*/
