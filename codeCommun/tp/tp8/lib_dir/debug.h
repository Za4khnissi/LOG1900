#define F_CPU 8000000
#include <avr/io.h>
#include <avr/eeprom.h>

//#define DEBUG // à mettre en commentaire au besoin

#ifdef DEBUG

#define INIT_DEBUG initialisationUART()
#define DEBUG_PRINT(x, y) transmissionMessage(x, y) // ou par RS-232
#else

#define INIT_DEBUG do {} while (0)
#define DEBUG_PRINT(x, y) do {} while (0) // code mort

#endif

void initialisationUART();
void transmissionUART(unsigned char data);
void transmissionMessage(const char etatActuel[], uint8_t taille);