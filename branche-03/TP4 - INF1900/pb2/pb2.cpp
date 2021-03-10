

#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

const uint8_t ROUGE = 0x02;
const uint8_t VERT = 0x01;
const uint8_t ETEINT = 0x00;

volatile uint8_t minuterieExpiree = 0;
volatile uint8_t boutonPoussoir = 0;

volatile uint16_t duree = 7812;

ISR(TIMER1_COMPA_vect)
{
    minuterieExpiree = 1;
}

ISR(INT0_vect)
{
    _delay_ms(30);
    if (PIND & (1 << PIND2))
    {
        boutonPoussoir = 1;
    }
}

void Minuterie(uint16_t duree)
{
    minuterieExpiree = 0;
    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A = 0 << WGM10 | 0 << WGM11;
    TCCR1B = 1 << CS12 | 1 << CS10 | 0 << CS11 | 1 << WGM12 | 0 << WGM13;
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}

void initialisation(void)
{
    cli();
    DDRA = 0xff;
    DDRD = 0x00;

    EIMSK |= (1 << INT0);

    EICRA |= (1 << ISC01) | (1 << ISC00);

    sei();
}

int main()
{
    initialisation();

    _delay_ms(10000);

    PORTA = ROUGE;

    _delay_ms(100);

    PORTA = ETEINT;

    Minuterie(duree);
    
    while (minuterieExpiree == 0 && boutonPoussoir == 0);

    // Une interruption s'est produite. Arrêter toute
    // forme d'interruption. Une seule réponse suffit.   

    cli();
    // Verifier la réponse
    if(minuterieExpiree == 0)
    {
        PORTA = VERT;
    }
    else
    {
        PORTA = ROUGE;
    }
    
    return 0;
}
