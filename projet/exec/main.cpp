#define F_CPU 8000000UL
#include <avr/interrupt.h>

#include "Can.h"
#include "UART.h"
#include "debug.h"
#include "Motor.h"
#include "Sensor.h"
#include "Converter.h"
#include "Maneuver.h"
#include "Keyboard.h"
#include "Display.h"
#include "Time.h"

#define CONV_FACTOR 5.0F / 255.0F
#define DEFAULT_FREQUENCE 7812
#define ONE_ 1


volatile uint8_t maneuverId;
volatile uint8_t lecture = 0;


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

    moteur(-35, 35);
    _delay_ms(1500);

    moteur(35, 35);
    _delay_ms(2000);

    moteur(35, -35);
    _delay_ms(1500);

    moteur(35, 35); 

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

ISR(TIMER1_COMPA_vect)
{
    lecture = 1;

}

ISR (INT0_vect) {

    if(maneuverId == 1) {
        char mot[] = "Manoeuvre 1 (OK - ATTENTION - ATTENTION) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver1();
    }

    else if(maneuverId == 2){
        char mot[] = "Manoeuvre 2 ( ATTENTION - ATTENTION - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver2();
    }

    else if(maneuverId == 3) {
        char mot[] = "Manoeuvre 3 ( DANGER - DANGER - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver3();
    }

    else if(maneuverId == 4) {
        char mot[] = "Manoeuvre 4 (OK - OK - DANGER) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver4();
    }

    else if(maneuverId == 5){
        char mot[] = "Manoeuvre 5 (DANGER - OK - OK) \n";
        DEBUG_PRINT(mot, sizeof(mot));
        maneuver5();
    }
    else if(maneuverId == 0) {
        //Invalid Maneuver
        char mot[] = "Manoeuvre invalid \n";
        DEBUG_PRINT(mot, sizeof(mot));
    }
    
    EIFR |= (1 << INTF0) ;
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

// Mode demarrage
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

/** Vide un tableau de caracteres passe en parametre
 * @param array tableau a vider
 * @param size taille du tableau a vider 
 */
void clear(char array[], uint8_t size)
{
    
    for (uint8_t i = 0; i < size; i++)
    {
        array[i] = '\0';
    }
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


int main() {
    
    init();
    partirMinuterie0();

    uint8_t adc = 0;
    float voltage = 0.0F;

    float previousDist = 0.0F;
    float currentDist = 0.0F;
    
    float distances[3] = {0, 0, 0};
    bool distChanged = false;

    char categories[40] = "";
    bool categoryChanged = false;

    PressedButton  bouton;
    DisplayMode displayMode = DisplayMode::ON_FREQUENCE;

    uint16_t frequence = DEFAULT_FREQUENCE;

    AnalogDigConv converter = AnalogDigConv::INTERNAL;
    
    // Mode demarrage
    startUpMode();

    for(;;) {

        bouton = detectPressedButton();

        initFrequence();
        partirMinuterie(frequence);
        do {} while (lecture == 0);

        switch(bouton) {

            case PressedButton::ONE:
                frequence = DEFAULT_FREQUENCE;
                DEBUG_PRINT("Le bouton 1 du clavier a ete appuye.\n", 38);
            break;

            case PressedButton::TWO:
                frequence = DEFAULT_FREQUENCE / 2;
                DEBUG_PRINT("Le bouton 2 du clavier a ete appuye.\n", 38);
            break;

            case PressedButton::FOUR:
                frequence = DEFAULT_FREQUENCE / 4;
                DEBUG_PRINT("Le bouton 4 du clavier a ete appuye.\n", 38);
            break;
            
            case PressedButton ::R:
                DEBUG_PRINT("Le bouton R du clavier a ete appuye.\n", 38);
                displayMode = DisplayMode::ON_FREQUENCE;
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

            default:
                break;

        }
        
        for (uint8_t sensorIndex = 0; sensorIndex < 3; sensorIndex++)
        {
            previousDist = distances[sensorIndex];

            selectSensor(sensorIndex);
            adc = computeConversion(converter); 
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
            
            if (displayMode == DisplayMode::ON_CATEGORY_CHANGE)
            {
                if(selectCategory(previousDist) != selectCategory(currentDist))
                {
                    categoryChanged = true;
                }
            }
            
            strcat(categories, selectCategory(currentDist)); 

            // Si on a pas encore pris les mesures sur tous les capteurs, celui de droite etant le dernier, 
            // rajouter un separateur
            if (sensorIndex != Sensor::RIGHT)
            {
                strcat(categories, " | ");
            } 
        }

        maneuverId = selectManeuver(distances);
        addManeuverId(categories, maneuverId);

        switch (displayMode)
        {
            case DisplayMode::ON_FREQUENCE:

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
                break;
            }

        clear(categories, sizeof(categories));
    }
    return 0;
}