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

void Utils::setAllDDR(bool output){
    if (output){
        DDRA = 0xff; 
        DDRB = 0xff; 
        DDRC = 0xff; 
        DDRD = 0xff; 
    }
    else{
        DDRA = 0x00; 
        DDRB = 0x00; 
        DDRC = 0x00; 
        DDRD = 0x00; 
    }
}



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

void Utils::turnOnMatrix(uint8_t operande){
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


void Utils::turnOffMatrix(){
    PORTA = 0x00;
}

void Utils::turnOffDirection() {
    PORTB &= ~_BV(PB2) & ~_BV(PB3) & ~_BV(PB4) & ~_BV(PB5) & ~_BV(PB6) & ~_BV(PB7);
}

void Utils::directionNorth() {
    PORTB |= _BV(PB2) | _BV(PB4) | _BV(PB5); // 0011 0100
}

void Utils::directionSouth(){
    PORTB |= _BV(PB2) | _BV(PB4) | _BV(PB7); // 1001 0100 
}

void Utils::directionEast() {
    PORTB |= _BV(PB2) | _BV(PB3) | _BV(PB6); // 0100 1100
}

void Utils::directionWest() {
    PORTB |= _BV(PB3) | _BV(PB4) | _BV(PB6); // 0101 1000
}

void Utils::turnOffDisplay() {
    PORTC = 0x00;
}