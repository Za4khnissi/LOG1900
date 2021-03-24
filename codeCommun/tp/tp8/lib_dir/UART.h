#define F_CPU 8000000UL
#include <avr/io.h>

namespace UART
{
    void initializeUART ( void );
    void transmissionUART ( uint8_t data );
}
