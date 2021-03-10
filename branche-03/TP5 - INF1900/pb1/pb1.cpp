
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>


const uint8_t ROUGE = 0x02;
const uint8_t VERT = 0x01;
const uint8_t ETEINT = 0x00;

int main(){
    DDRA = 0xff;
    DDRD = 0x00;

    uint8_t* adresse = 0x0000;
    char chaine[] = "POLYTECHNIQUE MONTREAL";
    uint8_t taille = sizeof(chaine);
    for (uint8_t i = 0; i < taille; i++){
        eeprom_write_byte	(adresse, chaine[i]);
        adresse++;
    }
    adresse = 0x0000;
    
    char chaineRelue[taille];
    for (uint8_t i = 0; i < taille; i++){
        chaineRelue[i] = eeprom_read_byte(adresse);
        adresse++;
    }

    bool identiques = false;
    for (uint8_t i = 0; i < taille; i++){
        if(chaineRelue[i] == chaine[i]){
            identiques = true;
        }
        else{
            identiques = false;
            break;
        }
    }
    if (identiques){
        PORTA = VERT;
    }
    else{
        PORTA = ROUGE;
    }
    return 0;
}

