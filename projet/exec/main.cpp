/**
 *                                    INF1900 - PROJET FINAL
 * 
 *              Robot qui détecte des obstacles et qui effectue des manœuvres d'évitement
 * 
 * AUTEURS: Einstein Franck Tiomo Epongo
 *          Moussa Abdillahi Daoud   
 *          Zabiullah Shair Zaie
 *          Zakarya Khnissi
 * 
 * EQUIPE:  0320
 * 
 *                                  IDENTIFICATIONS MATÉRIELLES :
 * 
 *       -----------------------------------------------------------------------------------------
 *       |      Broche       |    Direction  |                   Utilisation                     |  
 *       |---------------------------------------------------------------------------------------|
 *       |        A0         |       OUT     | Choix du can externe / interne                    |
 *       |---------------------------------------------------------------------------------------|
 *       |        A1         |       IN      | Transmettre la tension au can interne             |
 *       |---------------------------------------------------------------------------------------|
 *       |      A2 - A3      |       OUT     | Sélectionner les capteurs                         | 
 *       |---------------------------------------------------------------------------------------|
 *       | A4 – A5 – A6 – A7 |       OUT     | Afficheurs 7 segments                             |     
 *       |---------------------------------------------------------------------------------------|                
 *       |       B0          |       OUT     | Output enable du can externe                      |         
 *       |---------------------------------------------------------------------------------------|
 *       |       B1          |       IN      | Recevoir les bits de la conversion du can externe |
 *       |---------------------------------------------------------------------------------------|
 *       |   B2 – B3 – B4    |	     OUT	 | Sélectionner les valeurs à l’entrée du can externe|
 *       |---------------------------------------------------------------------------------------|
 *       |       B5          |	     OUT     | Input enable du 1er afficheur                     |
 *       |---------------------------------------------------------------------------------------|
 *       |       B6	         |       OUT     | Input Enable du 2ème afficheur                    |
 *       |---------------------------------------------------------------------------------------|
 *       |       B7          |	     OUT     | Output Enable des afficheurs gauches              |
 *       |---------------------------------------------------------------------------------------|
 *       |       C0	         |       OUT	 | Input Enable du 3ème afficheur                    |
 *       |---------------------------------------------------------------------------------------|
 *       |       C1	         |       OUT     | Input Enable du 4ème afficheur                    |
 *       |---------------------------------------------------------------------------------------|
 *       |   C2 – C3 – C4    |		 IN      | Entrées des claviers                              |
 *       |---------------------------------------------------------------------------------------|
 *       |   C5 – C6 – C7	 |       OUT     | Sorties des claviers                              |
 *       |---------------------------------------------------------------------------------------|
 *       |       D2	         |       IN      | INT0                                              |
 *       |---------------------------------------------------------------------------------------|
 *       |      D3 – D6 	 |       OUT	 | Directions des roues gauches et droite            |
 *       |---------------------------------------------------------------------------------------|
 *       |     D4 – D5	     |       OUT	 | OC1B et OC1A                                      |
 *       |---------------------------------------------------------------------------------------|
 *   
 */

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
#include "Time.h"

#define CONV_FACTOR 5.0F / 255.0F
#define DEFAULT_FREQUENCE 7812
#define MSG_SIZE 38
#define MAX_SIZE 40

volatile bool maneuverModeActive = false;

volatile uint8_t lecture = 0;
volatile bool hexadecimalDisplay = false; 


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

    maneuverModeActive = true;
    EIFR |= (1 << INTF0);
}

void startTimer0 () {

    TCNT0 = 0;
    OCR0A = 78; 
    TCCR0A |= (1 << WGM01);
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 |= (1 << OCIE0A);
}

void startTimer(uint16_t duree)
{

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
    const char baudRate[] = " 9600 bps\n";
    DEBUG_PRINT(baudRate, sizeof(baudRate));
    
    leftDisplays7(AB, true); 
    rightDisplays7(CD, true);

    Motor motor;

    motor.forward(254, 254);
    _delay_ms(1000); 
    motor.backward(254, 254);
    _delay_ms(1000);
    motor.stop();

    display7off(true);
    
}

/** Vide un tableau de caracteres passe en parametre
 * @param array tableau a vider
 * @param size taille du tableau a vider 
 */
void clearCharArray(char array[], uint8_t size)
{
    
    for (uint8_t i = 0; i < size; i++)
    {
        array[i] = '\0';
    }
}

void init ( void ) {
    cli ();

    
    DDRA = 0xFD;  
    DDRB = 0xFD; 
    DDRC |= (1 << PC0) | (1 << PC1);

    DDRD = 0x78; 

    // permettre les interruptions externes
    EIMSK |= (1 << INT0) ;

    // sensibiliser les interruptions externes aux
    // changements de niveau du bouton-poussoir
    EICRA |= (1 << ISC01) | (1 << ISC00) ;

    sei ();
}

void initFrequency(){
    cli();
    DDRD &= ~_BV(PD4) | ~_BV(PD5);
    sei();
}

int main() {
    
    init();
    startTimer0();

    uint8_t maneuverId = 0;

    uint8_t adc = 0;
    float voltage = 0.0F;

    float previousDist = 0.0F;
    float currentDist = 0.0F;
    
    float distances[3] = {0, 0, 0};
    bool distChanged = true;

    char categories[MAX_SIZE] = "";
    bool categoryChanged = true;

    PressedButton  bouton;
    DisplayMode displayMode = DisplayMode::ON_FREQUENCE;

    uint16_t frequence = DEFAULT_FREQUENCE;

    AnalogDigConv converter = AnalogDigConv::INTERNAL;
    

    startUpMode();

    for(;;) {

        bouton = detectPressedButton();

        initFrequency();    
        startTimer(frequence);

        do {
            bouton = detectPressedButton();
        } while (lecture == 0 && bouton == PressedButton::REPEAT);

        switch(bouton) {

            case PressedButton::ONE:
                frequence = DEFAULT_FREQUENCE;
                DEBUG_PRINT("Le bouton 1 du clavier a ete appuye.\n", MSG_SIZE);
            break;

            case PressedButton::TWO:
                frequence = DEFAULT_FREQUENCE / 2;
                DEBUG_PRINT("Le bouton 2 du clavier a ete appuye.\n", MSG_SIZE);
            break;

            case PressedButton::FOUR:
                frequence = DEFAULT_FREQUENCE / 4;
                DEBUG_PRINT("Le bouton 4 du clavier a ete appuye.\n", MSG_SIZE);
            break;
            
            case PressedButton::R:
                DEBUG_PRINT("Le bouton R du clavier a ete appuye.\n", MSG_SIZE);
                displayMode = DisplayMode::ON_FREQUENCE;
            break;

            case PressedButton::V:
                DEBUG_PRINT("Le bouton V du clavier a ete appuye.\n", MSG_SIZE);
                displayMode = DisplayMode::ON_DISTANCE_CHANGE;
            break;

            case PressedButton::C:
                DEBUG_PRINT("Le bouton C du clavier a ete appuye.\n", MSG_SIZE);
                displayMode = DisplayMode::ON_CATEGORY_CHANGE;
            break;

            case PressedButton::I:
                DEBUG_PRINT("Le bouton I du clavier a ete appuye.\n", MSG_SIZE);
                converter = AnalogDigConv::INTERNAL;
            break;

            case PressedButton::E:
                DEBUG_PRINT("Le bouton E du clavier a ete appuye.\n", MSG_SIZE);
                converter = AnalogDigConv::EXTERNAL;
            break;

            case PressedButton::HASHTAG:
                DEBUG_PRINT("Le bouton # du clavier a ete appuye.\n", MSG_SIZE);
                hexadecimalDisplay = !hexadecimalDisplay;
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

                display(distances, categories);
                break;
                
            
            case DisplayMode::ON_DISTANCE_CHANGE:

                if (distChanged)
                {
                    display(distances, categories);
                    distChanged = false;
                }
                break;

            case DisplayMode::ON_CATEGORY_CHANGE:
            
                if(categoryChanged)
                {
                    display(distances, categories);
                    categoryChanged = false;
                }
                break;

            default:
                break;
            }

        if(maneuverModeActive)
        {
            executeManeuver(maneuverId, hexadecimalDisplay);   
            maneuverModeActive = false;
        }

        clearCharArray(categories, sizeof(categories));
    }
    return 0;
}