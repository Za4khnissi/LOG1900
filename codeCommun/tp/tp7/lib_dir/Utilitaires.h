#include <avr/io.h>

enum EtatPin {
    LOW, 
    HIGH 
};

namespace Utilitaires
{
    void definirEtatPin(volatile uint8_t &port, uint8_t pins[], EtatPin pinState[], uint8_t nbPins);
    void allumerDEL(uint8_t &port, uint8_t couleur);
}