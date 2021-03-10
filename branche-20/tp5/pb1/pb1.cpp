#include <avr/io.h>
#include <avr/eeprom.h>

//void eeprom_write_block (const void *__src, void *__dst, size_t __n);
//void eeprom_read_block (void *__dst, const void *__src, size_t __n);

//void eeprom_write_byte (uint8_t * adresse, uint8_t value);
//uint8_t eeprom_read_byte (const uint8_t * adresse   );


void lumiereEteinte(){
    PORTA = 0x00;
}
void lumiereVerte(){
    PORTA = 0x02;
}
void lumiereRouge(){
    PORTA = 0x01;
}

//by byte
//start from first EEPROM cell
#define EEADDR 0
//block size to be copied
#define BLKSIZE 45

//data will be written to EEPROM (by byte)
#define EEDATA 100


//message to be written to EEPROM (by block)
uint8_t message[] ="*P*O*L*Y*T*E*C*H*N*I*Q*U*E* *M*O*N*T*R*E*A*L*";
//uint8_t message1[] ="*P*O*L*Y*T*E*C*H*N*I*Q*U*E* *M*O*N*T*R*E*A***";


int main(void){
    DDRA = 0xff; // PORT A est en mode sortie

    lumiereEteinte();

    /************by block*******************/
    bool sameChar = true;
    //where block has to be read
    uint8_t readblock[BLKSIZE];
    //write block EEPROM
    eeprom_write_block((const void *)message, (void *)EEADDR, BLKSIZE);
    //read block from EEPROM
    eeprom_read_block ((void *)readblock, (const void *)EEADDR, BLKSIZE);

    for(int i = 0; i < 45 && sameChar; i++) {
        if(message[i] != readblock[i])
            sameChar = false;
    }

    if(sameChar) { lumiereVerte(); }
    else { lumiereRouge(); }


    /************by byte********************
    //put EEPROM data to this variable
    volatile uint8_t EEByte;
    //write EEPROM
    eeprom_write_byte ((uint8_t*)EEADDR, EEDATA);
    //read from EEPROM
    EEByte = eeprom_read_byte((uint8_t*)EEADDR);
    
    if(EEByte == EEDATA) { lumiereVerte(); }
    else { lumiereRouge(); }
    ***************************************/

    return 0;
}