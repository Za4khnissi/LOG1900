#include <avr/eeprom.h>
#include <avr/io.h>
#define F_CPU 8000000
#include <util/delay.h>
#include "debug.h"
#include "../tp7/lib_dir/Utils.cpp"

enum Instruction{
    DBT = 0X01;
    ATT = 0X2;
    DAL = 0X88;
}

DAL 0x05;
DBT ;



void fncDAL(uint8_t operande){
    if(operande =< 127){
    Utils::setDELColor(&PORTA, GREEN);
    }
    if(operande ­=> 128){
    Utils::setDELColor(&PORTA, RED);

    }
}

int main(){


        while(true){
            nextInstruction();
            switch (command)
            {
            case
                break;

            case DAL:
            fncDAL(operande);
                break;

            case DAL:
                
                break;

            case /* constant-expression */:
                /* code */
                break;

            default:
                break;
            }

        }
    return 0;
};