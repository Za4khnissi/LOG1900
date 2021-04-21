#ifndef MANEUVER_H
#define MANEUVER_H
#include <avr/io.h>
#define MIN_DISTANCE 10.0F

enum Maneuver {
    M1,
    M2,
    M3,
    M4,
    M5,
    Invalid
};

uint8_t checkCategory(float distance) {
    if (distance >= MIN_DISTANCE && distance < 20.0F)
        return 1;
    else if (distance >= 20.0F && distance < 50.0F)
        return 2;
    else
        return 3;
}

/** Selectionne la categorie a laquelle appartient une distance
 * 
 * @param distance distance dont on veut evaluer la categorie
 * @return categorie a laquelle appartient la distance passee en parametre
 * 
 */
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