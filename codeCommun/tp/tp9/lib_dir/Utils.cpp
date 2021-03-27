#include <avr/io.h>
#define F_CPU 8000000
#include <avr/interrupt.h>
#include <util/delay.h>

#define OFF   0
#define GREEN 1
#define RED   2
#define AMBRE 3
#define DIX_UNITE 50

#define THRESHOLD 4
#define JUMP      10

enum PinState {
    LOW, 
    HIGH 
};


void setPinState(volatile uint8_t &port, uint8_t pins[], PinState pinState[], uint8_t nbPins)
{
    for(uint8_t i = 0; i < nbPins; i++)
    {
        if (pins[i] > 7) {
            continue; // Numéro de pin invalide, prochaine pin
        }
        port = (pinState[i] == PinState::HIGH) ? port | _BV(pins[i]) : port & ~_BV(pins[i]);
    }
}

void setDELColor(uint8_t &port, uint8_t color)
{
    if (color == AMBRE)
    {
        static bool val = false;
        if(val)
            port = GREEN;
        else {
            port = RED;
        }
        val = !val;
    }
    else
        port = color;
}

void dynamic_delay_ms(uint16_t delay)
{
    if (delay < THRESHOLD)
    {
        for (uint16_t i = 0; i < delay; i++)
        {
            _delay_ms(1);
        }
    } else 
    {
        uint16_t jump = delay >> 2;
        for (uint16_t i = 0; i < delay; i+= jump)
        {
            for (uint8_t j = 0; j < jump / 10; j++)
            {
                _delay_ms(10);
            }
            
        }
    }

}



void allumerMatrice(uint8_t operande){
    //Test
    for(int i = 0; i < 3; i++){
        PORTA = 0x0F;
        _delay_ms(500);
        PORTA = 0x00;
        _delay_ms(500);
    }
    for(int i = 0; i < 10; i++){
        PORTA = 0x00;
        _delay_ms(DIX_UNITE);
        PORTA = 0xE6;
        _delay_ms(DIX_UNITE);
        PORTA = 0xC6;
        _delay_ms(DIX_UNITE);
        PORTA = 0x86;
        _delay_ms(DIX_UNITE);
        PORTA = 0x06;
        _delay_ms(DIX_UNITE);
    } //Fin test
  
    PORTA = operande;
}


void eteindreMatrix(){
    PORTA = 0x00;
}

void eteindreDirection() {
    PORTB = 0x00;
}

void matrix(uint8_t operande){
    PORTA = operande;
}

void directionNord(){
    PORTB = 0x34;
}

void directionSud(){
    PORTB = 0x94;
}

void directionEst(){
    PORTB = 0x4C;
}

void directionOuest(){
    PORTB = 0x58;
}