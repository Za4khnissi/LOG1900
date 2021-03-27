#pragma once

#ifndef DEL_H
#define DEL_H

#include <avr/io.h>

#define OFF   0
#define GREEN 1
#define RED   2
#define AMBRE 3

class Del
{
private:
    bool val = false;

public:
    Del();
    ~Del();

    void setDelColor(volatile uint8_t *port, uint8_t color);
};

#endif //DEL_H

