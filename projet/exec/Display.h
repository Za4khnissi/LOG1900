#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include <util/delay.h>
#include "Time.h"

#define MAX_DISPALY_ARRAY_SIZE 73
#define BUFFER_SIZE 9
#define AB 171
#define CD 205


Time time;

enum DisplayMode
{
    ON_FREQUENCE,
    ON_DISTANCE_CHANGE,
    ON_CATEGORY_CHANGE
};


void display7off(bool off) 
{
    if(off)
    {
        PORTB |= (1 << PORTB7);
        PORTD |= (1 << PORTD7);
        PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    }
    else {
        PORTB &= ~(1 << PORTB7);
        PORTD &= ~(1 << PORTD7);
        PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);    
    }
}

void leftDisplays7(uint8_t vitesse, bool hexNum) {
    uint8_t left;
    uint8_t right;

    if(hexNum) {
        left = vitesse / 16;
        right = vitesse % 16;
    }
    else {
    right = vitesse % 10;
    left = (vitesse - right) / 10;
    }

    PORTB &= ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC0) & ~(1 << PORTC1);
    PORTB = (1 << PORTB5);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= left << 4;
    _delay_ms(50);

    PORTB &= ~(1 << PORTB5);
    PORTC &= ~(1 << PORTC0) & ~(1 << PORTC1);
    PORTB = (1 << PORTB6);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= right << 4;;
    _delay_ms(50);
}

    
void rightDisplays7(uint8_t vitesse, bool hexNum) {
    uint8_t left;
    uint8_t right;

    if(hexNum) {
        left = vitesse / 16;
        right = vitesse % 16;
    }
    else {
    right = vitesse % 10;
    left = (vitesse - right) / 10;
    }

    PORTB &= ~(1 << PORTB5) & ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC1);
    PORTC = (1 << PORTC0);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= left << 4;
    _delay_ms(50);
    
    PORTB &= ~(1 << PORTB5) & ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC0);
    PORTC = (1 << PORTC1);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= right << 4;
    _delay_ms(50);
}

/** Fonction qui affiche les detections faites par le robot a travers le temps
 * 
 * @param distances distances relevees par les capteurs de distances
 * @param categories categories auxquelles appartiennent chacun des distances
 * @param converter conversisseur utilisee pour faire les 
 * conversions analogiques --> numeriques
 *
 * */
void display(float distances[], const char categories[])
{
    char buffer[MAX_DISPALY_ARRAY_SIZE] = "";
    char numbersBuffer[BUFFER_SIZE] = "";
    const char *sides[] = {"G:", "C:", "D:"};

    char elapsedTime[13] = "";
    time.concatenateElapsedTime(elapsedTime);
    strcat(buffer, elapsedTime);

    float currentValue = 0.0F;

    for (uint8_t i = 0; i < 3; i++)
    {
        currentValue = distances[i];
        
        // partie entiere
        uint8_t ipart = (uint8_t)currentValue;
        
        // partie flottante
        float fpart = (currentValue - ipart) * 10;
        uint8_t fracPart = (uint8_t)fpart;
    
        sprintf(numbersBuffer, "%d.%d ", ipart, fracPart);

        strcat(buffer, sides[i]);
        strcat(buffer, numbersBuffer);

    }

    strcat(buffer, "- ");
    strcat(buffer, categories);
    DEBUG_PRINT(buffer, MAX_DISPALY_ARRAY_SIZE);
}


#endif