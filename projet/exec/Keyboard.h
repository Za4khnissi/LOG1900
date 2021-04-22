#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <avr/io.h>

enum PressedButton {
    ONE,
    TWO,
    FOUR,
    R,
    V,
    C,
    I,
    E,
    HASHTAG,
    REPEAT
};

PressedButton detectPressedButton() {

        PressedButton bouton = PressedButton::REPEAT;
        DDRC = 0xE3;
        PORTC = 0xE0;
    
        if (PINC & 0x04) { 
            DDRC = 0x1F; 
            PORTC = 0x04; 

                if (PINC & 0x80){ 
					while(PINC & 0x80){};
                    bouton = PressedButton::HASHTAG;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = PressedButton::C;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = PressedButton::FOUR;
                }

        }
        else if (PINC & 0x08){
            DDRC = 0x1F;
            PORTC = 0x08;

                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = PressedButton::E;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = PressedButton::V;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = PressedButton::TWO;
                }

        }
        else if (PINC & 0x10){ 
            DDRC = 0x1F;
            PORTC = 0x10;
            
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = PressedButton::I; 
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = PressedButton::R;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = PressedButton::ONE;
                }
		}
        return bouton;
}

#endif