#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
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
    MAR2 = 0x61,
    MAV = 0x62,
    MRE = 0x63,
    TRD = 0x64,
    TRG = 0x65,
    DBC = 0xC0,
    FBC = 0xC1,
    FIN = 0xFF,

};

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


//fonction qui recoit les instructions de Uart pour l'ecrire sur la memoire

uint16_t writeEEPROM() {
    
    UART uart;

    uint8_t* address = 0;
    uint16_t instructionSize = 0;


    uint8_t byte1 = uart.receiveUART();
    
    //on ecrit le premier octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, byte1);
    address++;

    //On recoit maintenant le 2e octet de UART
    uint8_t byte2 = uart.receiveUART();

    //on ecrit le deuxieme octet recu depuis le UART sur la memoire eeprom
    eeprom_write_byte(address, byte2);
    address++;

    //le nombre d'instruction sont stocke dans instructionSize
    instructionSize = ((byte1 << 8) | byte2);

    for(uint16_t i = 2; i < instructionSize; i++) {
        //Donnee recue de UART (1byte)
        uint8_t dataFromUART = uart.receiveUART();

        //On ecrit les donnes recues sur la memoire EEPROM
        eeprom_write_byte(address, dataFromUART);

        //On increment l'adresse
        address++;
    }
    return instructionSize;
}

void readEEPROM(uint16_t instructionSize){

    UART uart;

    Led led;

    Motor motor;

    uint8_t instruction = 0;

    uint8_t* loopAdress = 0;

    uint8_t operand = 0;

    uint8_t loopCounter = 0;

    //variable boolenne retournant le debut de l'execution des instructions
    //pour verifier si l'instruction dbt(debut) est executee avant de rentrer
    //dans le switch statement
    bool startCode = false;

    uint8_t* programCounter = 0;

    for(uint16_t i = 0; i < instructionSize; i++){
        
        instruction = eeprom_read_byte(programCounter);
        programCounter++;

        //Afficheur 7segments
        PORTC = instruction; //affichage de l instruction courante
        _delay_ms(1000);

        operand = eeprom_read_byte(programCounter);
        programCounter++;

        uart.transmissionUART(instruction);

        if(instruction == DBT) {
            DEBUG_PRINT("Instruction: DBT\n", 18);
            startCode = true;
        }   

        if(startCode) {

            switch (instruction) {

                case ATT:
                    DEBUG_PRINT("Instruction: ATT\n", 18);
                    attendre(operand);

                    break;

                case DAL:
                    DEBUG_PRINT("Instruction: DAL\n", 18);
                    turnOnLED(led, operand);
                    break;

                case DET:
                    DEBUG_PRINT("Instruction: DET\n", 18);
                    turnOffLED(led);

                    break;

                case MON:
                    DEBUG_PRINT("Instruction: MON\n", 18);
                    Utils::turnOnMatrix(operand);
                    break;

                case MOF:
                    DEBUG_PRINT("Instruction: MOF\n", 18);
                    Utils::turnOffMatrix();
                    break;

                case MAR:
                case MAR2:
                    DEBUG_PRINT("Instruction: MAR\n", 18);

                        motor.stop();
                        Utils::turnOffDirection();
                    break;

                case MAV:
                    DEBUG_PRINT("Instruction: MAV\n", 18);
                    motor.forward(operand, operand);

                    Utils::turnOffDirection();
                    Utils::directionNorth();
                    break;

                case MRE:
                    DEBUG_PRINT("Instruction: MRE\n", 18);
                    motor.backward(operand, operand);

                    Utils::turnOffDirection();
                    Utils::directionSouth();
                    break;

                case TRD:
                    DEBUG_PRINT("Instruction: TRD\n", 18);
                    motor.turnRight();
                    
                    Utils::turnOffDirection();
                    Utils::directionEast();
                    break;

                case TRG:
                    DEBUG_PRINT("Instruction: TRG\n", 18);
                    motor.turnLeft();

                    Utils::turnOffDirection();
                    Utils::directionWest();
                    break;

                case DBC:
                    DEBUG_PRINT("Instruction: DBC\n", 18);
                    //loopAdress nous indique l'adresse du debut de la boucle
                    loopAdress = programCounter;
                    //on met dans loopCounter le nombre d'iteration de la boucle plus une fois
                    loopCounter = operand + ONE_; 
                    break;

                case FBC:
                    DEBUG_PRINT("Instruction: FBC\n", 18);
                    //On verifie si le nombre d'iteration est termine
                    if(loopCounter > ONE_) {
                        programCounter = loopAdress; // Revenir a l'adresse de DBC
                        loopCounter--;
                    }
                    break;

                case FIN:
                    DEBUG_PRINT("Instruction: FIN\n", 18);
                    motor.stop();
                    Utils::turnOffDirection();
                    Utils::turnOffMatrix();
                    _delay_ms(2000);
                    Utils::turnOffDisplay();
                    turnOffLED(led);
                    return;
            }
        }   
    }
}




int main() {

    bool output = true;
    //Initialisation des entrees/sorties
    Utils::setAllDDR(output);

    //Ici on fait appel a la fonction receiveInstructions 
    //qui ecrit les donnees recues par UART sur la memoire
    //et par la suite nous retourne la taille des instructions
    uint16_t instructionSize = writeEEPROM();

    readEEPROM(instructionSize);
}




















