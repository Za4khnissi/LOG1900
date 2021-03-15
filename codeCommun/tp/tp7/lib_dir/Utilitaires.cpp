#include <avr/io.h> 
#include "Utilitaires.h"

#define VERT 1
#define ROUGE 2
#define AMBRE 3

void Utilitaires::definirEtatPin(volatile uint8_t &port, uint8_t pins[], EtatPin pinState[], uint8_t nbPins)
{
    for(uint8_t i = 0; i < nbPins; i++)
    {
        if (pins[i] > 7) {
            continue; // Numéro de pin invalide, prochaine pin
        }
        port = (pinState[i] == EtatPin::HIGH) ? port | _BV(pins[i]) : port & ~_BV(pins[i]);
    }
}

void Utilitaires::allumerDEL(uint8_t &port, uint8_t couleur)
{
    if (couleur == AMBRE)
    {
        static bool val = false;
        if(val)
            port = VERT;
        else{
            port = ROUGE;
        }
        val = !val;
    }
    else
        port = couleur;
}

void debounce();

/*


*/