#ifndef SENSOR_H
#define SENSOR_H

#include <avr/io.h>

enum Sensor
{
    LEFT,
    CENTER,
    RIGHT
};

/** Active le fonctionnement d'un capteur en faisant les configurations
 * necessaires sur les registres 
 * 
 * @param sensor capteur que l'on veut activer
 * 
 */
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


#endif //SENSOR_H