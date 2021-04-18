#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>

#include "Can.h"
#include "UART.h"
#include "debug.h"
#include "Motor.h"

#define BUFFER_SIZE 20
#define MIN_DISTANCE 10.0F
#define CONV_FACTOR 5.0F / 255.0F

enum pressedBouton{
    ONE,
    TWO,
    FOUR,
    R,
    V,
    C,
    I,
    E,
    HASHTAG
};


enum Sensor
{
    left,
    center,
    right
};

enum AnalogDigConv
{
    internal = 12,
    external
};

void selectSensor(uint8_t sensor)
{
    switch (sensor)
    {
    case left:
        PORTA &= ~((1 << PA2) | (1 << PA3));
        break;

    case center:
        PORTA |= (1 << PA2);
        PORTA &= ~(1 << PA3);
        break;

    case right:
        PORTA |= (1 << PA3);
        PORTA &= ~(1 << PA2);
        break;
    }
}

void selectCan(AnalogDigConv can)
{
    switch (can)
    {
    case internal:
        PORTA &= ~(1 << PA0);
        break;

    case external:
        PORTA |= (1 << PA0);
        break;
    }
}

uint8_t distance(AnalogDigConv converter)
{
    selectCan(converter);
    if (converter == AnalogDigConv::internal)
    {
        Can can;
        uint8_t volts = can.lecture(PA1) >> 2;
        return volts; 
    }
    else
    {
        uint8_t result = 0;
        PORTB &= ~(1 << PB2) & ~(1 << PB3) & ~(1 << PB4);

        PORTB |= (1 << PB2) | (1 << PB3) | (1 << PB4); //MUX D7
        _delay_ms(10);
        result = PINB & (1 << PB1);       //0000 0001
        PORTB |= (1 << PB3) | (1 << PB4); //MUX D6
        PORTB &= ~(1 << PB2);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 1; //0000 0011

        PORTB |= (1 << PB2) | (1 << PB4); //MUX D5
        PORTB &= ~(1 << PB3);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 2;

        PORTB |= (1 << PB4); //MUX D4
        PORTB &= ~((1 << PB2) | (1 << PB3));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 3;

        PORTB |= (1 << PB2) | (1 << PB3); //MUX D3
        PORTB &= ~(1 << PB4);
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 4;

        PORTB |= (1 << PB3); //MUX D2
        PORTB &= ~((1 << PB2) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 5;

        PORTB |= (1 << PB2); //MUX D1
        PORTB &= ~((1 << PB3) | (1 << PB4));
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 6;

        PORTB &= ~((1 << PB2) | (1 << PB3) | (1 << PB4)); //MUX D0
        _delay_ms(10);
        result += (PINB & (1 << PB1)) << 7;
        return result >> 1;
    }
}

void display(float distances[], const char categories[])
{
    float currentValue = 0.0F;
    const char msg[] = "Can interne: ";
    char buffer[BUFFER_SIZE] = {' '};
    const char *sides[] = {"G:", "C:", "D:"};

    DEBUG_PRINT(msg, sizeof(msg));

    for (uint8_t i = 0; i < 3; i++)
    {
        currentValue = distances[i];
        // partie entiere
        uint8_t ipart = (uint8_t)currentValue;
        // partie flottante
        float fpart = (currentValue - ipart) * 10;
        uint8_t fracPart = (uint8_t)fpart;
        sprintf(buffer, "%d.%d ", ipart, fracPart);

        DEBUG_PRINT(sides[i], sizeof(sides[i]));
        DEBUG_PRINT(buffer, BUFFER_SIZE);
    }

    DEBUG_PRINT(categories, 40);
}

const char *selectCategory(float distance)
{
    const char *category = "";

    if (distance >= MIN_DISTANCE && distance < 20.0F)
    {
        category = "DANGER";
    }
    else if (distance >= 20.0F && distance < 50.0F)
    {
        category = "ATTENTION";
    }
    else
    {
        category = "OK";
    }
    return category;
}

void eteindreAfficheurs() 
{
    PORTB |= (1 << PORTB7);
    PORTD |= (1 << PORTD7);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
}

void afficheursDemarrage() {

    //Affichage de A sur le premier afficheur
    PORTB &= ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC0) & ~(1 << PORTC1);
    PORTB = (1 << PORTB5);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= (1 << PORTA7) | (1 << PORTA5);
    _delay_ms(50);
    //Affichage de B sur le 2e afficheur
    PORTB &= ~(1 << PORTB5);
    PORTC &= ~(1 << PORTC0) & ~(1 << PORTC1);
    PORTB = (1 << PORTB6);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= (1 << PORTA7) | (1 << PORTA5) | (1 << PORTA4);
    _delay_ms(50);
    //Affichage de C sur le 3e afficheur
    PORTB &= ~(1 << PORTB5) & ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC1);
    PORTC = (1 << PORTC0);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= (1 << PORTA7) | (1 << PORTA6);
    _delay_ms(50);
    //Affichage de D sur le 4e afficheur
    PORTB &= ~(1 << PORTB5) & ~(1 << PORTB6);
    PORTC &= ~(1 << PORTC0);
    PORTC = (1 << PORTC1);
    PORTA &= ~(1 << PORTA7) & ~(1 << PORTA6) & ~(1 << PORTA5) & ~(1 << PORTA4);
    PORTA |= (1 << PORTA7) | (1 << PORTA6) | (1 << PORTA4);
    _delay_ms(50);

    _delay_ms(2000);

    //Eteindre afficheurs 
    eteindreAfficheurs();
}

void startUpMode() 
{
    const char baudRate[] = "2400 bps\n";
    DEBUG_PRINT(baudRate, sizeof(baudRate));
    
    afficheursDemarrage();
    
    Motor motor;

    motor.forward(254, 254);
    _delay_ms(1000); // Timer?
    motor.backward(254, 254);
    _delay_ms(1000); // Timer?
    motor.stop();
    
}

pressedBouton pressButton(){

        pressedBouton bouton;
        DDRC = 0xE0;
        PORTC = 0xE0;
        //DDRC |= _BV(PC7) | _BV(PC6) | _BV(PC5) 
        //DDRC &= ~_BV(PC4) & ~_BV(PC3) & ~_BV(PC2);
        //PORTC |= _BV(PC7) | _BV(PC6) | _BV(PC5);

        if (PINC & 0x04){
            DDRC = 0x1C;
            PORTC = 0x04;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC2);
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = pressedBouton::HASHTAG;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = pressedBouton::C;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = pressedBouton::FOUR;
                }

        }
        if (PINC & 0x08){
            DDRC = 0x1C;
            PORTC = 0x08;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC3);
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = pressedBouton::E;
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = pressedBouton::V;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = pressedBouton::TWO;
                }

        }
        if (PINC & 0x10){ 
            DDRC = 0x1C;
            PORTC = 0x10;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC4);
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = pressedBouton::I; 
                }
                else if (PINC & 0x40){
					while(PINC & 0x40){};
                    bouton = pressedBouton::R;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = pressedBouton::ONE;
                }
		}
        return bouton;
}

void detection() {

uint8_t adc = 0;
    float voltage = 0.0F;
    float previousDist = 0.0F;
    float currentDist = 0.0F;
    
    float distances[3] = {0, 0, 0};
    char categories[40] = "";
    while (true)
    {
        uint8_t sensorIndex = 0;
        for (; sensorIndex < 3; sensorIndex++)
        {
            previousDist = distances[sensorIndex]; 
            selectSensor(sensorIndex);
            // permettre d echager de can après un appui sur le clavier
            adc = distance(AnalogDigConv::internal); 
            voltage = adc * CONV_FACTOR;
            currentDist = 28.998 * pow(voltage, -1.141);
            if (currentDist <= MIN_DISTANCE)
            {
                currentDist = MIN_DISTANCE;
            }
            distances[sensorIndex] = currentDist;
            
            // concatenation
            strcat(categories, selectCategory(currentDist)); 
            if (sensorIndex != Sensor::right)
            {
                strcat(categories, "|");
            } else {
                strcat(categories, "\n"); 
            }
            
            switch (displayMode)
            {
                case DisplayMode::R:
                    /* code */
                    break;
                
                case DisplayMode::V:
                    if (!equals(currentDist, previousDist))
                    {
                        display(distances, categories);
                        // vider le tableau categorie
                        for (uint8_t i = 0; i < sizeof(categories); i++)
                        {
                            categories[i] = '\0';
                        }
                        _delay_ms(100);
                        }
                    
                    break;
                case DisplayMode::C:
                    break;
                default:
                    break;
            }
            
            
        }
        
    }

}



int main() {
    DDRA |= ~_BV(PA1); //1111 1101 
    DDRB |= ~_BV(PB1); //1111 1101
    //DDRD |= ~_BV() ; //1111 1000
    //0000 0011

    
    pressedBouton bouton;
    
    
    // Mode demarrage
    startUpMode();

    for(;;){
        pressButton();
        bouton = pressButton();

        switch(bouton){
            case pressedBouton::ONE:
                DEBUG_PRINT("Le bouton 1 du clavier a été appuyé.\n", 41);
            break;
            case pressedBouton::TWO:
                DEBUG_PRINT("Le bouton 2 du clavier a été appuyé.\n", 41);
            break;
            case pressedBouton::FOUR:
                DEBUG_PRINT("Le bouton 4 du clavier a été appuyé.\n", 41);
            break;
            case pressedBouton::R:
                DEBUG_PRINT("Le bouton R du clavier a été appuyé.\n", 41);

            break;
            case pressedBouton::V:
                DEBUG_PRINT("Le bouton V du clavier a été appuyé.\n", 41);

            break;
            case pressedBouton::C:
                DEBUG_PRINT("Le bouton C du clavier a été appuyé.\n", 41);

            break;
            case pressedBouton::I:
                DEBUG_PRINT("Le bouton I du clavier a été appuyé.\n", 41);
                selectCan(AnalogDigConv::internal);
            break;
            case pressedBouton::E:
                DEBUG_PRINT("Le bouton E du clavier a été appuyé.\n", 41);
                selectCan(AnalogDigConv::external);
            break;
            case pressedBouton::HASHTAG:
                DEBUG_PRINT("Le bouton # du clavier a été appuyé.\n", 41);
            break;
        }
    }
    return 0;
}