#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main()
{
    /*
    0xF5 : Hexadecimal base 16
    0b11110101: Biniure base 2
    255: Decimal base 10
    */
    //        #PIN: 76543210

    /*
    DDRx: Mettre les pins en entrer et en sortie
    PORTx: Changer les valeurs des pins pour HIGH (1) et LOW
    PINx: Lire les valeurs des pins
    */
    
    DDRB = 0x03; //| 0b00000011;
    
    const uint16_t b1 = 60; // frequence est set a 1kHz
    const uint16_t b2 = 400;

    const uint32_t time = 10000000; // microsecondes = 3 s

    const uint32_t nbPeriodes1 = time / b1; // 3000 periodes
    const uint32_t nbPeriodes2 = time / b2;

    const uint8_t nombreDeCycles = 5; // 100% 75% 50% 25% 0% 

    const uint16_t periodesParCycles1 =  nbPeriodes1 / nombreDeCycles; // 600
    const uint16_t periodesParCycles2 =  nbPeriodes2 / nombreDeCycles;



    while(true)
    {
        uint16_t a1 = b1;  
        uint16_t a2 = b2;


        for(uint8_t cycle = 0; cycle < nombreDeCycles; cycle++) 
        {
            uint16_t c1 = b1 - a1; 
            
            for(uint16_t i = 0; i < periodesParCycles1; i++)  
            {
                PORTB = 0x01;       
                for(uint8_t i = 0; i < c1 / 10; i++) 
                    _delay_us(10);

                PORTB = 0x00;
                for(uint8_t i = 0; i < a1 / 10; i++)
                    _delay_us(10);
            }
            a1 -= b1 / (nombreDeCycles - 1); 
        }


        for(uint8_t cycle = 0; cycle < nombreDeCycles; cycle++) 
        {
            uint16_t c2 = b2 - a2; 
            
            for(uint16_t i = 0; i < periodesParCycles2; i++)  
            {
                PORTB = 0x03;
                for(uint8_t i = 0; i < c2 / 10; i++) 
                    _delay_us(10);

                PORTB = 0x00;
                for(uint8_t i = 0; i < a2 / 10; i++)
                    _delay_us(10);
            }
            a2 -= b2 / (nombreDeCycles - 1); 
        }
    }
    
    return 0;
}