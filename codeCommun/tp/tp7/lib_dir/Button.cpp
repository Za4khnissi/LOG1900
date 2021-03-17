#include "Button.h"

// constructeur : initialisation du bouton
Button::Button(ButtonDDR buttonDDR, uint8_t pin)
{
    switch (buttonDDR)
    {
    case pseudoDDRA:
        DDRA &= ~(1 << pin);
        break;
    case pseudoDDRB:
        DDRB &= ~(1 << pin);
        break;
    case pseudoDDRC:
        DDRC &= ~(1 << pin);
        break;
    case pseudoDDRD:
        DDRD &= ~(1 << pin);
        break;
    }
} 

Button::Button(ButtonDDR btnDDR[], uint8_t pins[], uint8_t nbButtons){

    for (uint8_t i = 0; i < nbButtons; i++)
    {
        Button(btnDDR[i], pins[i]); 
    }
}


void Button::initialization(PseudoPin pseudoPin, InterruptSenseControl ics)
{
    cli();

    switch (pseudoPin) //Remember to also handle the case when the button is PB2
    {
        case pseudoPD2:
            EIMSK |= (1 << INT0);
            switch (ics)
            {
                case InterruptSenseControl::LOW_LEVEL:
                    // Nothing to do
                    break;
                case InterruptSenseControl::ANY_EDGE:
                    EICRA |= (0 << ISC01) | (1 << ISC00);
                    break;
                case InterruptSenseControl::FALLING_EDGE:
                    EICRA |= (1 << ISC01) | (0 << ISC00);
                    break;
                case InterruptSenseControl::RISING_EDGE:
                    EICRA |= (1 << ISC01) | (1 << ISC00);
                    break;    
            }
            break;

        case pseudoPD3:
            EIMSK |= (1 << INT1);
            switch (ics)
            {
                case InterruptSenseControl::LOW_LEVEL:
                    // Nothing to do
                    break;
                case InterruptSenseControl::ANY_EDGE:
                    EICRA |= (0 << ISC11) | (1 << ISC10);
                    break;
                case InterruptSenseControl::FALLING_EDGE:
                    EICRA |= (1 << ISC11) | (0 << ISC10);
                    break;
                case InterruptSenseControl::RISING_EDGE:
                    EICRA |= (1 << ISC11) | (1 << ISC10);
                    break;    
            }
            break;

        case pseudoPB2:
            EIMSK |= (1 << INT2);
            switch (ics)
            {
                case InterruptSenseControl::LOW_LEVEL:
                    // Nothing to do
                    break;
                case InterruptSenseControl::ANY_EDGE:
                    EICRA |= (0 << ISC21) | (1 << ISC20);
                    break;
                case InterruptSenseControl::FALLING_EDGE:
                    EICRA |= (1 << ISC21) | (0 << ISC20);
                    break;
                case InterruptSenseControl::RISING_EDGE:
                    EICRA |= (1 << ISC21) | (1 << ISC20);
                    break;    
            }
            break; 
    }
    sei();
}

bool Button::debounce(uint8_t &pin, uint8_t pressedButton) {
    
    bool isPressed = false; 
    if (pin & (1 << pressedButton)) {
        _delay_ms(30);
        if (pin & (1 << pressedButton))
            isPressed = true;
    }
    return isPressed;
}