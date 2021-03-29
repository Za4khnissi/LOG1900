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

#define ONE_ 1

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
uint16_t receiveInstructions() {
    
    UART uart;

    uint8_t* address = 0x00;
    uint16_t instructionSize;

    //Test Del

    //
    uint8_t byte1 = uart.receive();
    
    //on ecrit le premier octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, byte1);
    address++;

    //On recoit maintenant le 2e octet de UART
    uint8_t byte2 = uart.receive();

    //on ecrit le deuxieme octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, byte2);
    address++;

    //le nombre d'instruction sont stocke dans instructionSize
    instructionSize = ((byte1 << 8) | byte2);

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
void turnOnLED(Led led, uint8_t operand){
    if(operand <= 127){
        led.setLedColor(&PORTB, GREEN);
    }
    else {
        led.setLedColor(&PORTB, RED);

    }
}

//Instruction: det
void turnOffLED(Led led){
    led.setLedColor(&PORTB, OFF);
}

//Instruction: att
void attendre(uint8_t operand) {
    _delay_ms(25*operand);
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
    uint8_t* programCounter = 0x00;

    //adresse de la boucle
    uint8_t* loopAdress;

    //compteur de boucle
    uint8_t loopCounter = 0;

    //Variable qui stocke la valeur retourne lors de la lecture de EEPROM
    uint8_t instruction = 0;

    //Variable qui stock l'operand qui suit l'instruction
    uint8_t operand = 0;

    //Ici on fait appel a la fonction receiveInstructions 
    //qui ecrit les donnees recues par UART sur la memoire
    //et par la suite nous retourne la taille des instructions
    uint16_t instructionSize = receiveInstructions();

    //instanciation des objets le cas echeants
    

    //variable boolenne retournant le debut de l'execution des instructions
    //pour verifier si l'instruction dbt(debut) est executee avant de rentrer
    //dans le switch statement
    bool execCode = false;


    for(uint16_t i = 0; i < instructionSize; i++){

        //lecture de l'instruction
        instruction = eeprom_read_byte(programCounter);
        programCounter++;

        //Afficheur 7segments
        PORTC = instruction; //affichage de l instruction courante
        _delay_ms(1000);

        operand = eeprom_read_byte(programCounter);
        programCounter++;

        //on transmet vers pc l'instruction lue
        uart.transmissionUART(instruction);

        //on verifie si l'instruction est debut avant de rentrer 
        //dans le switch statement dans la premiere fois sinon on fait rien
        if(instruction == DBT)
            execCode = true;


        if(execCode){

            switch (instruction) {

                case ATT:
                    DEBUG_PRINT(("Instruction attendre...\n"));
                    attendre(operand);

                    break;

                case DAL:
                    DEBUG_PRINT(("Instruction allumer Del...\n"));
                    turnOnLED(led, operand);
                    break;

                case DET:
                    DEBUG_PRINT(("Instruction eteindre Del...\n"));
                    turnOffLED(led);

                    break;

                case MON:
                    DEBUG_PRINT(("Instruction allumer matrix Del...\n"));
                    Utils::turnOnMatrix(operand);
                    break;

                case MOF:
                    DEBUG_PRINT(("Instruction eteindre matrix Del...\n"));
                    Utils::turnOffMatrix();
                    break;

                case MAR:
                case MBR:
                    DEBUG_PRINT(("Instruction arreter moteur...\n"));
                    if(operand == 0x60 || operand == 0x61) {

                        motor.stop();
                    }
                    break;

                case MAV:
                    DEBUG_PRINT(("Instruction avancer moteur...\n"));
                    motor.forward(operand, operand);

                    Utils::turnOffDirection();
                    Utils::directionNorth();
                    break;

                case MRE:
                    DEBUG_PRINT(("Instruction reculer moteur...\n"));
                    motor.backward(operand, operand);

                    Utils::turnOffDirection();
                    Utils::directionSouth();
                    break;

                case TRD:
                    DEBUG_PRINT(("Instruction tourner moteur a droite...\n"));
                    motor.turnRight();
                    
                    Utils::turnOffDirection();
                    Utils::directionEast();
                    break;

                case TRG:
                    DEBUG_PRINT(("Instruction tourner moteur a gauche...\n"));
                    motor.turnLeft();

                    Utils::turnOffDirection();
                    Utils::directionWest();
                    break;

                case DBC:
                    DEBUG_PRINT(("Instruction debut boucle...\n"));
                    //programCounter nous indique l'adresse du debut de la boucle
                    loopAdress = programCounter;
                    //on met dans loopCounter le nombre d'iteration de la boucle plus une fois
                    loopCounter = operand + ONE_;
                    break;

                case FBC:
                    DEBUG_PRINT(("Instruction fin boucle...\n"));
                    //On verifie si le nombre d'iteration est termine
                    if(loopCounter > 0) {
                        programCounter = loopAdress; // Revenir a l'adresse de DBC
                        loopCounter--;
                    }
                    break;

                case FIN:
                    DEBUG_PRINT(("Instruction fin tout court...\n"));
                    motor.stop();
                    Utils::turnOffDirection();
                    Utils::turnOffMatrix();
                    _delay_ms(2000);
                    Utils::turnOffDisplay();
                    turnOffLED(led);
                    return 0;
                    break;    

            }
        }   
    }   
    //return 0;
}


