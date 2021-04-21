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

#define DEFAULT_FREQUENCE 7812

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

enum Sensor
{
    LEFT,
    CENTER,
    RIGHT
};

enum AnalogDigConv
{
    INTERNAL = 12,
    EXTERNAL
};

enum DisplayMode
{
    ON_FREQUENCY = 20,
    ON_DISTANCE_CHANGE,
    ON_CATEGORY_CHANGE,
    INIT
};


struct Time {
    uint8_t min = 0, sec = 0, cent = 0;
};

volatile Time time;

ISR(TIMER0_COMPA_vect){

    time.cent++;

    if(time.cent == 100) {
        time.sec++;
        time.cent = 0;
    }
    if(time.sec == 60){
        time.min++;
        time.sec = 0;
    }
}

void partirMinuterie0 () {
    // mode CTC du timer 0 avec horloge divisée par 1024
    // interruption après la durée spécifiée
    TCNT0 = 0;
    OCR0A = 78; 
    TCCR0A |= (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);
}

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
    case LEFT:
        PORTA &= ~((1 << PA2) | (1 << PA3));
        break;

    case CENTER:
        PORTA |= (1 << PA2);
        PORTA &= ~(1 << PA3);
        break;

    case RIGHT:
        PORTA |= (1 << PA3);
        PORTA &= ~(1 << PA2);
        break;
    }
}

void selectCan(AnalogDigConv can)
{
    switch (can)
    {
    case INTERNAL:
        PORTA &= ~(1 << PA0);
        break;

    case EXTERNAL:
        PORTA |= (1 << PA0);
        break;
    }
}

uint8_t distance(AnalogDigConv converter)
{
    selectCan(converter);
    if (converter == AnalogDigConv::INTERNAL)
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


void displayElapsedTime()
{
    char min[4];
    char sec[4];
    char cent[4];

    sprintf(min, "%d", time.min);
    strcat(min, ":");

    sprintf(sec, "%d", time.sec);
    strcat(sec, ".");

    sprintf(cent, "%d", time.cent);
    strcat(cent, " - ");

    if(time.min < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    DEBUG_PRINT(min, 2);
    
    if(time.sec < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    
    DEBUG_PRINT(sec, 2);

    if(time.cent < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    DEBUG_PRINT(cent, 2); 
}

void display(float distances[], const char categories[], AnalogDigConv converter)
{
    //displayElapsedTime();
    
    char min[20];
    char sec[20];
    char cent[20];

    sprintf(min, "%d", time.min);
    strcat(min, ":");

    sprintf(sec, "%d", time.sec);
    strcat(sec, ".");

    sprintf(cent, "%d", time.cent);
    strcat(cent, " - ");

    if(time.min < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    DEBUG_PRINT(min, sizeof(min));
    
    if(time.sec < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    
    DEBUG_PRINT(sec, sizeof(sec));

    if(time.cent < 10)
    {
        DEBUG_PRINT("0", 2);
    }
    DEBUG_PRINT(cent, sizeof(cent));

    float currentValue = 0.0F;
    const char msg[] = "Can interne: ";
    const char msg2[] = "Can externe: ";
    char buffer[BUFFER_SIZE] = {' '};
    const char *sides[] = {"G:", "C:", "D:"};

    //if(converter == AnalogDigConv::INTERNAL) { DEBUG_PRINT(msg, sizeof(msg)); }
    //else { DEBUG_PRINT(msg2, sizeof(msg2)); }


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
    DEBUG_PRINT("- ", 3);
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
    const char baudRate[] = "9600 bps\n";
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

PressedButton detectPressedButton(){

        PressedButton bouton;
        DDRC = 0xE0;
        PORTC = 0xE0;
        //DDRC |= _BV(PC7) | _BV(PC6) | _BV(PC5) 
        //DDRC &= ~_BV(PC4) & ~_BV(PC3) & ~_BV(PC2);
        //PORTC |= _BV(PC7) | _BV(PC6) | _BV(PC5);

        if (PINC & 0x04) { // 0000 0100
            DDRC = 0x1C;
            PORTC = 0x04;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC2);

                if (PINC & 0x80){ // 1000 0000
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
            DDRC = 0x1C;
            PORTC = 0x08;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC3);
                if (PINC & 0x80){
					while(PINC & 0x80){};
                    bouton = PressedButton::E;
                }
                else if (PINC & 0x40){
					//while(PINC & 0x40){};
                    _delay_ms(300);
                    if(PINC & 0x40)
                        bouton = PressedButton::V;
                }
                else if (PINC & 0x20){
					while(PINC & 0x20){};
                    bouton = PressedButton::TWO;
                }

        }
        else if (PINC & 0x10){ 
            DDRC = 0x1C;
            PORTC = 0x10;
            //DDRC |= ~_BV(PC7) | ~_BV(PC6) | ~_BV(PC5) | _BV(PC4) | _BV(PC3) | _BV(PC2);
            //PORTC |= _BV(PC4);
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
        else{
            bouton = PressedButton::REPEAT;
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

void moteur(int8_t ocr1b, int8_t ocr1a)
{
    if(ocr1a < 0)
    {
        PORTD |= _BV(PD3);                 
        PORTD &= ~_BV(PD6);                
        ocr1b = (ocr1b*255)/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }

    else if((ocr1b > 0) & (ocr1a > 0))
    {
        PORTD |= _BV(PD3);            
        PORTD |= _BV(PD6);                  
        ocr1b = (ocr1b*255)/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if(ocr1b < 0)
    {
        PORTD &= ~_BV(PD3);                 
        PORTD |= _BV(PD6);                  
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (ocr1a*255)/100;
    }

    else if((ocr1a < 0) & (ocr1b < 0))
    {
        PORTD &= ~_BV(PD3);                 
        PORTD &= ~_BV(PD6);                  
        ocr1b = (-1*(ocr1b*255))/100;
        ocr1a = (-1*(ocr1a*255))/100;
    }
    PWM::adjustPWM(ocr1a, ocr1b);
}

void maneuver1()
{

    char mot[] = "Manoeuvre 1 (OK - ATTENTION - ATTENTION) \n";
    DEBUG_PRINT(mot, sizeof(mot));

    moteur(-35, 35);
    _delay_ms(1500);

    moteur(35, 35);
    _delay_ms(2000);

    moteur(35, -35);
    _delay_ms(1500);

    moteur(35, 35);         //pas necessaire mais juste pour respecter le consigne

    for(uint8_t i = 35; i <= 95; i = i+5)
    {
        moteur(i, i);
        _delay_ms(125);
    }

    _delay_ms(2000);
    moteur(0, 0);
}

void maneuver2()
{

    char mot[] = "Manoeuvre 2 ( ATTENTION - ATTENTION - OK) \n";
    DEBUG_PRINT(mot, sizeof(mot));

    moteur(35, -35);
    _delay_ms(1500);

    moteur(35, 35);
    _delay_ms(2000);

    moteur(-35, 35);
    _delay_ms(1500);

    moteur(35, 35);

    for(uint8_t i = 35; i < 95; i = i+5)
    {
        moteur(i, i);
        _delay_ms(125);
    }

    _delay_ms(2000);
    moteur(0, 0);
}

void maneuver3()
{
    char mot[] = "Manoeuvre 3 ( DANGER - DANGER - DANGER) \n";
    DEBUG_PRINT(mot, sizeof(mot));

    moteur(-50, -50);
    _delay_ms(1000);

    moteur(-70, 70);
    _delay_ms(1500);

    for(uint8_t i = 0; i <= 99; i = i+3)
    {
        moteur(i, i);
        _delay_ms(50);
    }

    for(uint8_t i = 99; i >= 74; i = i-5)
    {
        moteur(i, i);
        _delay_ms(500);
    }

    _delay_ms(2000);
    moteur(0, 0);
}

void maneuver4()
{

    char mot[] = "Manoeuvre 4 (OK - OK - DANGER) \n";
    DEBUG_PRINT(mot, sizeof(mot));
    moteur(78, 78);

    uint8_t i = 78;

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

void maneuver5()
{
    char mot[] = "Manoeuvre 5 (DANGER - OK - OK) \n";
    DEBUG_PRINT(mot, sizeof(mot));
    moteur(78, 78);

    uint8_t i = 78;

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

void maneuverI(){
    char mot[] = "Manoeuvre invalide \n";
    DEBUG_PRINT(mot, sizeof(mot));
}


ISR (INT0_vect) {

    if(maneuverId == 1) {
        maneuver1();
    }

    else if(maneuverId == 2){
        maneuver2();
    }

    else if(maneuverId == 3) {
        maneuver3();
    }

    else if(maneuverId == 4) {
        maneuver4();
    }

    else if(maneuverId == 5){
        maneuver5();
    }
    else if(maneuverId == 0) {
        maneuverI();
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
    DDRD &= ~_BV(PD4) | ~_BV(PD5);
    sei();
}

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

int main() {
    
    init();
    partirMinuterie0();

    uint8_t adc = 0;
    float voltage = 0.0F;

    float leftSensorDist, centerSensorDist, rightSensorDist;

    Maneuver maneuver;

    float previousDist = 0.0F;
    float currentDist = 0.0F;
    
    float distances[3] = {0, 0, 0};
    bool distChanged = false;

    char categories[40] = "";
    //uint8_t maneuverId = 0;
    bool categoryChanged = false;

    PressedButton  bouton;
    DisplayMode displayMode = DisplayMode::INIT;
    //Frequency frequency = O;

    uint16_t frequency = DEFAULT_FREQUENCE;

    PressedButton previousButton;
    AnalogDigConv converter = AnalogDigConv::INTERNAL;
    
    // Mode demarrage
    startUpMode();
    for(;;) {

        bouton = detectPressedButton();

        initFrequence();
        partirMinuterie(frequency);
        do {} while (lecture == 0);

        switch(bouton) {

            case PressedButton::ONE:
                frequency = DEFAULT_FREQUENCE;
                DEBUG_PRINT("Le bouton 1 du clavier a ete appuye.\n", 38);
            break;

            case PressedButton::TWO:
                frequency = DEFAULT_FREQUENCE / 2;
                DEBUG_PRINT("Le bouton 2 du clavier a ete appuye.\n", 38);
            break;

            case PressedButton::FOUR:
                frequency = DEFAULT_FREQUENCE / 4;
                DEBUG_PRINT("Le bouton 4 du clavier a ete appuye.\n", 38);
            break;
            
            case PressedButton ::R:
                DEBUG_PRINT("Le bouton R du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::ON_FREQUENCY;
            break;

            case PressedButton ::V:
                DEBUG_PRINT("Le bouton V du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::ON_DISTANCE_CHANGE;
            break;

            case PressedButton ::C:
                DEBUG_PRINT("Le bouton C du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::ON_CATEGORY_CHANGE;
            break;

            case PressedButton ::I:
                DEBUG_PRINT("Le bouton I du clavier a ete appuye.\n", 38);
                converter = AnalogDigConv::INTERNAL;
            break;

            case PressedButton ::E:
                DEBUG_PRINT("Le bouton E du clavier a ete appuye.\n", 38);
                converter = AnalogDigConv::EXTERNAL;
            break;

            case PressedButton ::HASHTAG:
                DEBUG_PRINT("Le bouton # du clavier a ete appuye.\n", 38);
            break;

        }
        
        for (uint8_t sensorIndex = 0; sensorIndex < 3; sensorIndex++)
        {
            previousDist = distances[sensorIndex];

            selectSensor(sensorIndex);
            adc = distance(converter); 
            voltage = adc * CONV_FACTOR;
            currentDist = 28.998F * pow(voltage, -1.141F);

            if (currentDist <= MIN_DISTANCE)
            {
                currentDist = MIN_DISTANCE;
            }
            
            if (previousDist  != currentDist)
            {
                distChanged = true;
                distances[sensorIndex] = currentDist;
            }
            
            if(selectCategory(previousDist) != selectCategory(currentDist))
            {
                categoryChanged = true;
            }

            // concatenation
            strcat(categories, selectCategory(currentDist)); 

            // Si on a pris les mesures sur tous les capteurs, celui de droite etant le dernier
            if (sensorIndex != Sensor::RIGHT)
            {
                strcat(categories, " | ");
            } 
        }

        maneuverId = selectManeuver(distances);
        addManeuverId(categories, maneuverId);

        switch (displayMode)
        {
            case DisplayMode::ON_FREQUENCY:

                display(distances, categories, converter);
                break;
                
            
            case DisplayMode::ON_DISTANCE_CHANGE:

                if (distChanged)
                {
                    display(distances, categories, converter);
                    distChanged = false;
                }
                break;

            case DisplayMode::ON_CATEGORY_CHANGE:
            
                if(categoryChanged)
                {
                    display(distances, categories, converter);
                    categoryChanged = false;
                }
                break;

            default:

                if(frequency == DEFAULT_FREQUENCE / 4) { // 250 ms
                    partirMinuterie( 3 * (DEFAULT_FREQUENCE / 4)); // 750 ms
                    do {} while (lecture == 0);
                }
                else if(frequency == DEFAULT_FREQUENCE / 2) { //500 ms
                    partirMinuterie(DEFAULT_FREQUENCE / 2); // 500 ms
                    do {} while (lecture == 0);
                }

                display(distances, categories, converter);
                break;
            }

        clear(categories, sizeof(categories));
    }
    return 0;
}