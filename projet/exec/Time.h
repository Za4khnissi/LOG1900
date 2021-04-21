#ifndef TIME_H
#define TIME_H

#include <avr/io.h>
#include <stdio.h>
#include <string.h>

struct Time {
    volatile uint8_t min = 0, sec = 0, cent = 0;

    /** Concatene les parametres de temps a afficher pour les mettre dans le format: mm::ss.cc
    *@param elapsedTime tableau servant de destination au resultat de la concatenation 
    */
    void concatenateElapsedTime(char* elapsedTime)
    {
        char buf[4];

        if(min < 10)
        {
            strcat(elapsedTime, "0");
        }

        sprintf(buf, "%d", min);
        strcat(elapsedTime, buf);

        strcat(elapsedTime, ":");

        if(sec < 10)
        {
            strcat(elapsedTime, "0");
        }

        sprintf(buf, "%d", sec);
        strcat(elapsedTime, buf);

        strcat(elapsedTime, ".");

        if(cent < 10)
        {
            strcat(elapsedTime, "0");
        }

        sprintf(buf, "%d", cent);
        strcat(elapsedTime, buf);

        strcat(elapsedTime, " - ");

    }
};

#endif