#ifndef CATEGORY_H
#define CATEGORY_H

#define MIN_DISTANCE 10.0F
#include <avr/io.h>

/** Determine la categorie d'une distance
 * 
 * @param distance distance dont on veut evaluer la categorie
 * @return numero de la categorie a la laquelle appartient la distance
 */
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

#endif