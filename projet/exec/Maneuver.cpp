#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "UART.h"
#include "debug.h"
#include "Motor.h"
#include "Led.h"
#include "Utils.h"

#define ONE_ 1


void moteur(int ocr1b, int ocr1a) //roue droite recule
{
    if(ocr1a < 0)
    {
        PORTD |= _BV(PD3);                 //gauche  ocr1b
        PORTD &= ~_BV(PD6);                //droite  ocr1a
        ocr1b = (ocr1b*255)/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }

    else if((ocr1b > 0) & (ocr1a > 0))
    {
        PORTD |= _BV(PD3);                 //gauche  ocr1b
        PORTD |= _BV(PD6);                  //droite  ocr1a
        ocr1b = (ocr1b*255)/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if(ocr1b < 0)
    {
        PORTD &= ~_BV(PD3);                 //gauche  ocr1b
        PORTD |= _BV(PD6);                  //droite  ocr1a
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if((ocr1a < 0) & (ocr1b < 0))
    {
        PORTD &= ~_BV(PD3);                 //gauche  ocr1b
        PORTD &= ~_BV(PD6);                  //droite  ocr1a
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }
    PWM::adjustPWM(ocr1a, ocr1b);
}

void spinHoraire(uint8_t ocr1b, uint8_t ocr1a) //roue gauche recule
{
    ocr1b = (ocr1b*255)/100;
    ocr1a = (ocr1a*255)/100;
    PORTD &= ~_BV(PD3);                 //gauche  ocr1b
    PORTD |= _BV(PD6);                  //droite  ocr1a
    PWM::adjustPWM(ocr1a, ocr1b);
}


void spinAntiHoraire(uint8_t ocr1b, uint8_t ocr1a) //roue droite recule
{
    ocr1b = (ocr1b*255)/100;
    ocr1a = (ocr1a*255)/100;
    PORTD |= _BV(PD3);                 //gauche  ocr1b
    PORTD &= ~_BV(PD6);                  //droite  ocr1a
    PWM::adjustPWM(ocr1a, ocr1b);
}

void avancer(uint8_t ocr1b, uint8_t ocr1a)
{
    ocr1b = (ocr1b*255)/100;
    ocr1a = (ocr1a*255)/100;
    PORTD |= _BV(PD3);
    PORTD |= _BV(PD6);
    PWM::adjustPWM(ocr1a, ocr1b);
}


/////////////////
void manoeuvre1()
{

    moteur(-35, 35);
    _delay_ms(1500);

    moteur(35, 35);
    _delay_ms(2000);

    moteur(35, -35);
    _delay_ms(1500);

    moteur(35, 35);         //pas necessaire mais juste pour respecter le consigne

    for(int i = 35; i <= 95; i = i+5)
    {
        moteur(i, i);
        _delay_ms(125);
    }

    _delay_ms(2000);
}



void manoeuvre2()
{

    moteur(35, -35);
    _delay_ms(1500);

    moteur(35, 35);
    _delay_ms(2000);

    moteur(-35, 35);
    _delay_ms(1500);

    moteur(35, 35);

    for(int i = 35; i < 95; i = i+5)
    {
        moteur(i, i);
        _delay_ms(125);
    }

    _delay_ms(2000);
}

void manoeuvre3()
{

    moteur(-50, -50);
    _delay_ms(1000);

    moteur(-70, 70);
    _delay_ms(1500);

    for(int i = 0; i <= 99; i = i+3)
    {
        moteur(i, i);
        _delay_ms(50);
    }

    for(int i = 99; i >= 74; i = i-5)
    {
        moteur(i, i);
        _delay_ms(500);
    }

    _delay_ms(2000);
}


void manoeuvre4()
{
    char buffer[10] = "";
    Motor motor;
    moteur(78, 78);

    int i = 78;
    DEBUG_PRINT("Ralentissement\n", 15);
    for(; i >= 48; i = i-2)
    {
        sprintf(buffer, "%d\n", i);
        DEBUG_PRINT(buffer, sizeof(buffer));
        moteur(i, 78);
        _delay_ms(250);
    }

    _delay_ms(1500);
    DEBUG_PRINT("---------------------\n", 15);
    DEBUG_PRINT("\n", 2);

    DEBUG_PRINT("Acceleration\n", 13);


    for(; i <= 78; i = i+2)
    {
        sprintf(buffer, "%d\n", i);
        DEBUG_PRINT(buffer, sizeof(buffer));
        moteur(i, 78);
        _delay_ms(250);
    }

    _delay_ms(2000);
    motor.stop();
}



void manoeuvre5()
{

    moteur(78, 78);

    int i = 78;

    for(; i >= 48; i = i-2)
    {
        moteur(78, i);
        _delay_ms(250);
    }

    _delay_ms(1500);


    for(; i <= 78; i = i+2)
    {
        moteur(78, i);
        _delay_ms(250);
    }

    _delay_ms(2000);
    
}

int main() {
    DDRD = 0xff;       

    manoeuvre4()    ;

    return 0;
}
