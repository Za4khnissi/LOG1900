#ifndef MANEUVER_H
#define MANEUVER_H
#include <avr/io.h>
#include "Display.h"
#include "Category.h"

void move(int8_t ocr1b, int8_t ocr1a)
{
    if((ocr1a < 0) && (ocr1b > 0))
    {
        PORTD |= _BV(PD3);                 
        PORTD &= ~_BV(PD6);                
        ocr1b = (ocr1b*255)/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }

    else if((ocr1b > 0) && (ocr1a > 0))
    {
        PORTD |= _BV(PD3);            
        PORTD |= _BV(PD6);                  
        ocr1b = (ocr1b*255)/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if((ocr1b < 0) && (ocr1a > 0))
    {
        PORTD &= ~_BV(PD3);                 
        PORTD |= _BV(PD6);                  
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if((ocr1a < 0) && (ocr1b < 0))
    {
        PORTD &= ~_BV(PD3);                 
        PORTD &= ~_BV(PD6);                  
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }
    PWM::adjustPWM(ocr1a, ocr1b);
}

void maneuver1(bool hex)
{
    display7off(false);

    move(-35, 35);
    leftDisplays7(35, hex);
    rightDisplays7(35, hex); 
    _delay_ms(1500);

    move(35, 35);
    _delay_ms(2000);

    move(35, -35);
    _delay_ms(1500);

    move(35, 35); 

    for(uint8_t i = 35; i <= 95; i = i+5)
    {

        move(i, i);
        leftDisplays7(i, hex);
        rightDisplays7(i, hex);
    
        _delay_ms(125);
    }

    _delay_ms(2000);
    move(0, 0);
    display7off(true);
}

void maneuver2( bool hex)
{
    display7off(false);

    move(35, -35);
    leftDisplays7(35, hex);
    rightDisplays7(35, hex);
    _delay_ms(1500);

    move(35, 35);
    _delay_ms(2000);

    move(-35, 35);
    _delay_ms(1500);

    move(35, 35);

    for(uint8_t i = 35; i <= 95; i = i+5)
    {
        move(i, i);
        leftDisplays7(i, hex);
        rightDisplays7(i, hex);
        _delay_ms(125);
    }

    _delay_ms(2000);
    move(0, 0);
    display7off(true);
}

void maneuver3(bool hex)
{
    display7off(false);

    move(-50, -50);
    leftDisplays7(50, hex);
    rightDisplays7(50, hex);
    _delay_ms(1000);

    move(-70, 70);
    leftDisplays7(70, hex);
    rightDisplays7(70, hex);
    _delay_ms(1500);

    for(uint8_t i = 0; i <= 99; i = i+3)
    {
        move(i, i);
        leftDisplays7(i, hex);
        rightDisplays7(i, hex);
        _delay_ms(50);
    }

    for(uint8_t i = 99; i >= 74; i = i-5)
    {
        move(i, i);
        leftDisplays7(i, hex);
        rightDisplays7(i, hex);
        _delay_ms(500);
    }

    _delay_ms(2000);
    move(0, 0);
    display7off(true);
}

void maneuver4(bool hex)
{
    display7off(false);

    move(78, 78);
    leftDisplays7(78, hex);
    rightDisplays7(78, hex);

    

    for(uint8_t i = 78; i >= 48; i = i-2)
    {
        move(i, 78);
        leftDisplays7(i, hex);
        rightDisplays7(78, hex);
        _delay_ms(250);
    }

    _delay_ms(1500);


    for(uint8_t i = 48; i <= 78; i = i+2)
    {
        move(i, 78);
        leftDisplays7(i, hex);
        rightDisplays7(78, hex);
        _delay_ms(250);
    }

    _delay_ms(2000);
    move(0, 0);
    display7off(true);

}

void maneuver5(bool hex)
{
    display7off(false);

    move(78, 78);
    leftDisplays7(78, hex);
    rightDisplays7(78, hex);

    

    for(uint8_t i = 78; i >= 48; i = i-2)
    {
        move(78, i);
        leftDisplays7(78, hex);
        rightDisplays7(i, hex);
        _delay_ms(250);
    }

    _delay_ms(1500);


    for(uint8_t i = 48; i <= 78; i = i+2)
    {
        move(78, i);
        leftDisplays7(78, hex);
        rightDisplays7(i, hex);
        _delay_ms(250);
    }

    _delay_ms(2000);
    move(0, 0);
    display7off(true);

}

/** Execute une manoeuvre selon le numero de celle ci
 * 
 * @param maneuverID numero de la manoeuvre a executer
 * @param hexadecimalDisplay determine si l affichage des vitesses doit 
 * etre hexadecimale ou decimale 
 */
void executeManeuver(uint8_t maneuverID, bool hexadecimalDisplay)
{
    if(maneuverID == 1) {
        char mot[] = "Manoeuvre 1 (OK - ATTENTION - ATTENTION) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver1(hexadecimalDisplay);
    }

    else if(maneuverID == 2){
        char mot[] = "Manoeuvre 2 ( ATTENTION - ATTENTION - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver2(hexadecimalDisplay);
    }

    else if(maneuverID == 3) {
        char mot[] = "Manoeuvre 3 ( DANGER - DANGER - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver3(hexadecimalDisplay);
    }

    else if(maneuverID == 4) {
        char mot[] = "Manoeuvre 4 (OK - OK - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver4(hexadecimalDisplay);
    }

    else if(maneuverID == 5){
        char mot[] = "Manoeuvre 5 (DANGER - OK - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver5(hexadecimalDisplay);
    }
    else if(maneuverID == 0) {

        //Invalid Maneuver
        char mot[] = "Manoeuvre invalide \n";
        DEBUG_PRINT(mot, sizeof(mot));
    }
}

/** Selectionne la manoeuvre a executer
 * 
 * @param distances distances mesurees par les capteurs
 * @return numero de la manoeuvre a executer
 */
uint8_t selectManeuver(float distances[])
{
    uint8_t index = 0;
    uint8_t left = checkCategory(distances[index]);
    uint8_t center = checkCategory(distances[index + 1]);
    uint8_t right = checkCategory(distances[index + 2]);

    if(left == 3 && center == 2 && right == 2) 
        return 1;   // Manoeuvre 1

    else if(left == 2 && center == 2 && right == 3)
        return 2;   // Manoeuvre 2

    else if(left == 1 && center == 1 && right == 1)
        return 3;   // Manoeuvre 3

    else if(left == 3 && center == 3 && right == 1)
        return 4;   // Manoeuvre 4

    else if(left == 1 && center == 3 && right == 3)
        return 5;   // Manoeuvre 5

    else
        return 0;
}

/** Concatene a le numero de manoevre correspondant a une configuration de categories
 * @param categries configuraion de categories
 * @param maneuverId numero de la manoeuvre
 */
void addManeuverId(char categories[], uint8_t maneuverId)
{
    switch (maneuverId)
    {
    case 1:
        strcat(categories, " (1)\n");
        break;
    case 2:
        strcat(categories, " (2)\n");
        break;
    case 3:
        strcat(categories, " (3)\n");
        break;
    case 4:
        strcat(categories, " (4)\n");
        break;
    case 5:
        strcat(categories, " (5)\n");
        break;
    
    default:
        strcat(categories, " (-)\n");
        break;
    }
}

#endif