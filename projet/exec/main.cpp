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
#define ONE_ 1


volatile uint8_t maneuverId;

volatile uint8_t lecture = 0;

enum Frequency{
    O, // one
    T, // two
    F  // four
};

enum Maneuver {
    M1,
    M2,
    M3,
    M4,
    M5,
    Invalid
};

enum pressedBouton{
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

enum DisplayMode
{
    RR = 20,
    VV,
    CC,
    INIT
};

void partirMinuterie(uint16_t duree)
{

    // mode CTC du timer 1 avec horloge divisée par 1024

    // interruption après la durée spécifiée
    lecture = 0;

    TCNT1 = 0;
    OCR1A = duree;
    TCCR1A = 1 << WGM12;
    TCCR1B = 1 << CS12 | 1 << CS10;
    TCCR1C = 0;
    TIMSK1 = 1 << OCIE1A;
}

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
        return volts; //* (5/1024);
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

void display(float distances[], const char categories[], AnalogDigConv converter)
{
    float currentValue = 0.0F;
    const char msg[] = "Can interne: ";
    const char msg2[] = "Can externe: ";
    char buffer[BUFFER_SIZE] = {' '};
    const char *sides[] = {"G:", "C:", "D:"};

    if(converter == AnalogDigConv::internal) { DEBUG_PRINT(msg, sizeof(msg)); }
    else { DEBUG_PRINT(msg2, sizeof(msg2)); }


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

void clear(char array[], uint8_t size)
{
    
    for (uint8_t i = 0; i < size; i++)
    {
        array[i] = '\0';
    }
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
        else if (PINC & 0x08){
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
        else if (PINC & 0x10){ 
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
        else{
            bouton = pressedBouton::REPEAT;
        }
        return bouton;
}

uint8_t checkCategory(float distance) {
    if (distance >= MIN_DISTANCE && distance < 20.0F)
        return 1;
    else if (distance >= 20.0F && distance < 50.0F)
        return 2;
    else
        return 3;
}

uint8_t selectManeuver(float leftDistance, float centerDistance, float rightDistance) {
    uint8_t left = checkCategory(leftDistance);
    uint8_t center = checkCategory(centerDistance);
    uint8_t right = checkCategory(rightDistance);

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
    moteur(0, 0);
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
    moteur(0, 0);
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
    moteur(0, 0);
}

void manoeuvre4()
{

    moteur(78, 78);

    int i = 78;

    for(; i >= 48; i = i-2)
    {
        moteur(i, 78);
        _delay_ms(250);
    }

    _delay_ms(1500);


    for(; i <= 78; i = i+2)
    {
        moteur(i, 78);
        _delay_ms(250);
    }

    _delay_ms(2000);
    moteur(0, 0);
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
    moteur(0, 0);
}


ISR (INT0_vect) {

    if(maneuverId == 1) {
        char mot[] = "Manoeuvre 1 (OK - ATTENTION - ATTENTION) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        manoeuvre1();
    }

    else if(maneuverId == 2){
        char mot[] = "Manoeuvre 2 ( ATTENTION - ATTENTION - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        manoeuvre2();
    }

    else if(maneuverId == 3) {
        char mot[] = "Manoeuvre 3 ( DANGER - DANGER - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        manoeuvre3();
    }

    else if(maneuverId == 4) {
        char mot[] = "Manoeuvre 4 (OK - OK - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        manoeuvre4();
    }

    else if(maneuverId == 5){
        char mot[] = "Manoeuvre 5 (DANGER - OK - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        manoeuvre5();
    }
    else if(maneuverId == 0) {
        //Invalid Maneuver
        char mot[] = "Manoeuvre invalid \n";
        DEBUG_PRINT(mot, sizeof(mot));
    }
    
    EIFR |= (1 << INTF0) ;
}

ISR(TIMER1_COMPA_vect)
{
    lecture = 1;

}

void init ( void ) {
    cli ();

    // Iniatialisation des ports
    DDRA = 0xFD; //1111 1101 
    DDRB = 0xFD; //1111 1101
    DDRD = 0xF8; //1111 1000

    // cette procédure ajuste le registre EIMSK
    // de l’ATmega324PA pour permettre les interruptions externes
    EIMSK |= (1 << INT0) ;

    // il faut sensibiliser les interruptions externes aux
    // changements de niveau du bouton-poussoir
    // en ajustant le registre EICRA
    EICRA |= (1 << ISC01) | (1 << ISC00) ;

    // sei permet de recevoir à nouveau des interruptions.
    sei ();
}

void initFrequence(){
    cli();
    DDRD &= ~_BV(PD4) & ~_BV(PD5);
    sei();
}

int main() {
    
    init();

    uint8_t adc = 0;
    float voltage = 0.0F;

    float leftSensorDist, centerSensorDist, rightSensorDist;

    Maneuver maneuver;

    float previousDist = 0.0F;
    float currentDist = 0.0F;
    
    float distances[3] = {0, 0, 0};
    bool distChanged = false;

    char categories[40] = "";
    bool categoryChanged = false;

    pressedBouton bouton;
    DisplayMode displayMode = DisplayMode::INIT;
    Frequency frequency = O;
    AnalogDigConv converter = AnalogDigConv::internal;
    
    // Mode demarrage
    startUpMode();
    for(;;){
        bouton = pressButton();

        switch(bouton){
            case pressedBouton::ONE:
                DEBUG_PRINT("Le bouton 1 du clavier a ete appuye.\n", 38);
                frequency = Frequency::O;
            break;
            case pressedBouton::TWO:
                DEBUG_PRINT("Le bouton 2 du clavier a ete appuye.\n", 38);
                frequency = Frequency::T;
            break;
            case pressedBouton::FOUR:
                DEBUG_PRINT("Le bouton 4 du clavier a ete appuye.\n", 38);
                frequency = Frequency::F;
            break;
            case pressedBouton::R:
                DEBUG_PRINT("Le bouton R du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::RR;
            break;
            case pressedBouton::V:
                DEBUG_PRINT("Le bouton V du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::VV;
            break;
            case pressedBouton::C:
                DEBUG_PRINT("Le bouton C du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::CC;
            break;
            case pressedBouton::I:
                DEBUG_PRINT("Le bouton I du clavier a ete appuye.\n", 38);
                converter = AnalogDigConv::internal;
            break;
            case pressedBouton::E:
                DEBUG_PRINT("Le bouton E du clavier a ete appuye.\n", 38);
                converter = AnalogDigConv::external;
            break;
            case pressedBouton::HASHTAG:
                DEBUG_PRINT("Le bouton # du clavier a ete appuye.\n", 38);
            break;
        }
        
        for (uint8_t sensorIndex = 0 ; sensorIndex < 3; sensorIndex++)
        {
            
            previousDist = distances[sensorIndex];
            selectSensor(sensorIndex);
            // permettre d echager de can après un appui sur le clavier
            adc = distance(converter); 
            voltage = adc * CONV_FACTOR;
            currentDist = 28.998F * pow(voltage, -1.141F);

            if(sensorIndex == 0) {
                leftSensorDist = currentDist;
            }
            else if(sensorIndex == 1) {
                centerSensorDist = currentDist;
            }
            else {
                rightSensorDist = currentDist;
            }

            maneuverId = selectManeuver(leftSensorDist, centerSensorDist, rightSensorDist);

            if (currentDist <= MIN_DISTANCE)
            {
                currentDist = MIN_DISTANCE;
            }
            
            if(selectCategory(previousDist) != selectCategory(currentDist))
            {
                categoryChanged = true;
            }

            if (previousDist  != currentDist)
            {
                distChanged = true;
                distances[sensorIndex] = currentDist;
            }
        

            // concatenation
            strcat(categories, selectCategory(currentDist)); 

            // Si on a pris les mesures sur tous les capteurs, celui de droite etant le dernier
            if (sensorIndex != Sensor::right){
                strcat(categories, "|");
            } 
            else {
                strcat(categories, "\n"); //
            }
        }

        switch (displayMode)
        {
            case DisplayMode::RR:
                while(bouton == pressedBouton::ONE || bouton == pressedBouton::TWO || bouton == pressedBouton::FOUR || bouton == pressedBouton::R){
                        
                    switch(bouton){
                        
                        case pressedBouton::ONE:
                        {
                        initFrequence();
                        while (bouton == pressedBouton::ONE || bouton == pressedBouton::REPEAT){
                            partirMinuterie(7812);
                            do{

                            }while(lecture == 0);
                                display(distances, categories, converter);

                            bouton = pressButton();
                        }
                        break;
                        }

                        case pressedBouton::TWO:
                        {
                            initFrequence();
                        while (bouton == pressedBouton::TWO || bouton == pressedBouton::REPEAT){
                            partirMinuterie(7812/2);
                            do{

                            }while(lecture == 0);
                                display(distances, categories, converter);
                            bouton = pressButton();
                        }
                        break;
                        }

                        case pressedBouton::FOUR:
                        {
                            initFrequence();
                        while (bouton == pressedBouton::FOUR || bouton == pressedBouton::REPEAT){
                            partirMinuterie(7812/4);
                            do{

                            }while(lecture == 0);
                                display(distances, categories, converter);
                            bouton = pressButton();
                        }
                        break;
                        }
                        default:
                        {
                            initFrequence();
                        while (bouton == pressedBouton::R || bouton == pressedBouton::REPEAT){
                            partirMinuterie(7812);
                            do{

                            }while(lecture == 0);
                                display(distances, categories, converter);
                            bouton = pressButton();
                        }
                        break;
                    }
                    bouton = pressButton(); 
                }
                break;
            
            case DisplayMode::VV:

                if (distChanged)
                {
                    display(distances, categories, converter);
                    distChanged = false;
                }
                break;

            case DisplayMode::CC:
            
                if(categoryChanged)
                {
                    display(distances, categories, converter);
                    categoryChanged = false;
                }
                break;

            default:
                break;
        }
        }

    clear(categories, sizeof(categories));
    }
    return 0;
}