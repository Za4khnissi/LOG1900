#include "Utils.h"

void Utils::setPinState(volatile uint8_t *port, uint8_t pins[], PinState pinState[], uint8_t nbPins)
{
    for(uint8_t i = 0; i < nbPins; i++)
    {
        if (pins[i] > 7) {
            continue; // Numéro de pin invalide, prochaine pin
        }
        setPinState(port, pins[i], pinState[i]);
    }
}

void Utils::setPinState(volatile uint8_t *port, uint8_t pin, PinState pinState)
{
    *port = (pinState == PinState::HIGH) ? *port | _BV(pin) : *port & ~_BV(pin);
}

/* 
void Utils::dynamic_delay_ms(uint16_t delay)
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
 */

void Utils::allumerMatrice(uint8_t operande){
    /* //Test
    for(int i = 0; i < 3; i++){
        PORTA = 0x0F;
        _delay_ms(500);
        PORTA = 0x00;
        _delay_ms(500);
    }
    for(int i = 0; i < 10; i++){
        PORTA = 0x00;
        _delay_ms(DELAY);
        PORTA = 0xE6;
        _delay_ms(DELAY);
        PORTA = 0xC6;
        _delay_ms(DELAY);
        PORTA = 0x86;
        _delay_ms(DELAY);
        PORTA = 0x06;
        _delay_ms(DELAY);
    } //Fin test */
    PORTA = operande;
}


void Utils::eteindreMatrix(){
    PORTA = 0x00;
}

void Utils::eteindreDirection() {
    PORTB = 0x00;
}

void Utils::matrix(uint8_t operande){
    PORTA = operande;
}

void Utils::directionNord(){
    PORTB = 0x34;
}

void Utils::directionSud(){
    PORTB = 0x94;
}

void Utils::directionEst(){
    PORTB = 0x4C;
}

void Utils::directionOuest(){
    PORTB = 0x58;
}
