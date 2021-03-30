#include "UART.h"
#include "string.h"
#include <avr/io.h>
#include <avr/eeprom.h>


#ifdef DEBUG

UART uart;

#define INIT_DEBUG UART uart
#define DEBUG_PRINT(x, y) uart.transmissionMessage(x, y) // ou par RS-232

#else

#define INIT_DEBUG do {} while (0)
#define DEBUG_PRINT(x, y) do {} while (0) 

#endif




