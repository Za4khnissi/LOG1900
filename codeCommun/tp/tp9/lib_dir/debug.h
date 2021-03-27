#include "UART.h"
#include "string.h"
#include <avr/io.h>
#include <avr/eeprom.h>

//#define DEBUG // à mettre en commentaire au besoin

#ifdef DEBUG
UART uart;

//#define INIT_DEBUG initialisationUART()
//DEBUG_PRINT(x, y) transmissionMessage(x, y) // ou par RS-232
#define DEBUG_PRINT( str);for (uint8_t i=0; i<strlen(str);i++){uart.transmissionUART(str[i]);}

#else

//#define INIT_DEBUG do {} while (0)
#define DEBUG_PRINT(str) do {} while (0) 

#endif

