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
    
    DDRA = 0x03; //| 0b00000011;
    DDRD = 0x00;
    
    const uint16_t b = 1000; // frequence est set a 1kHz
    const uint32_t time = 3000000; // microsecondes = 3 s
    const uint32_t nbPeriodes = time / b; // 3000 periodes
    const uint8_t nombreDeCycles = 5; // 100% 75% 50% 25% 0% 
    const uint16_t periodesParCycles =  nbPeriodes / nombreDeCycles; // 600



    while(true)
    {
        uint16_t a = b; // 1000 
        
        for(uint8_t cycle = 0; cycle < nombreDeCycles; cycle++) 
        {
            uint16_t c = b - a; // 0   |   250   |    500    |     750    |     1000
            
            for(uint16_t i = 0; i < periodesParCycles; i++)  
            {
                PORTA = 0x00;       
                for(uint8_t i = 0; i < c / 10; i++) 
                {
                    _delay_us(10);
                }

                PORTA = 0x01;
                for(uint8_t i = 0; i < a / 10; i++)
                {
                    _delay_us(10);
                }
            }
            a -= b / (nombreDeCycles - 1); 
        }
    }
    
    return 0;
}