
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

const uint8_t ROUGE = 0x02;
const uint8_t VERT = 0x01;
const uint8_t ETEINT = 0x00;


enum Etats {INIT, E1, E2};
volatile Etats etat = INIT;


ISR(INT0_vect)
{

    _delay_ms(30);

    switch (etat)
    {
        case INIT:
            etat = E1;
            break;
        case E1:
            etat = E2;
            break;
        case E2:
            etat = INIT;
            break;
    }

    EIFR |= (1 << INTF0);
}

void initialisation(void)
{
    // cli pour ne pas avoir dìnterruptions au debut du programme
    cli();
    DDRA = 0xff;
    DDRD = 0x00;
    // EIMSK pour les interruptions externes
    EIMSK |= (1 << INT0);
    // sensibilisation du bouton poussoir en agissant sur EICRA
    EICRA |= (1 << ISC01) | (1 << ISC00);
    // sei pour des nouvelles interruptions
    sei();
}

int main()
{
    initialisation();
    while (true)
    {
        switch (etat)
        {
        case INIT:
            PORTA = ETEINT;
            break;
        case E1:
            PORTA = VERT;
            break;
        case E2:
            PORTA = ROUGE;
            break;
        }
    }

    return 0;
}
