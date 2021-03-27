#define F_CPU 8000000UL
#include <avr/io.h>

class Del
{
private:

    enum DelColor {
        OFF,
        GREEN,
        RED,
        AMBRE,
    };

    bool val = false;

public:
    Del();
    ~Del();

    void setDelColor(volatile uint8_t *port, uint8_t color);
};


