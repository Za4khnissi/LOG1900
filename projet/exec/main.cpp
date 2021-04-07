void detectionRangee(uint8_t &port) {
	DDRC |= (1 << pin);
	PORTC = 0x1C;
	if (PINC & 0x20)
		int rangeeActive = R1;
	if (PINC & 0x40)
		int rangeeActive = R2;
	if (PINC & 0x80)
		int rangeeActive = R3;
}

enum Clavier
{
	C1, 
	C2, 
	C3,
	R1, 
	R2,
	R3
};
int colonneActive;
int rangeeActive;

void detectionColonne() {
	DDRC = 0xE0;

	PORTC |= 0xE0;

	if (PINC & 0x4)
		void detectionRangee(uint8_t & port);

	if (PINC & 0x8)
		void detectionRangee(uint8_t & port);

	}
	if (PINC & 0x16)
		void detectionRangee(uint8_t & port);





		//int colonneActive = C2;
		//DDRC = 0x1C;
		//PORTC = 0x1C;
		//if (PINC & 0x20)
		//	int rangeeActive = R1;
		//if (PINC & 0x40)
		//	int rangeeActive = R2;
		//if (PINC & 0x80)
		//	int rangeeActive = R3;
	};
}






void pressButton(){
	enum boutonAppuye {
		one,
		two,
		four,
		r,
		v,
		c,
		i,
		e,
		diez
	};

		boutonAppuye bouton;
		DDRC |= 0xE0;
		PORTC |= _BV(PC7) | _BV(PC6) | _BV(PC5);

		if (PINC & 0x04){
			DDRC |= 0x1C;
			PORTC &= 0x04;
				if (PINC & 0x80){
					bouton = diez; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = c;
				}
				else if (PINC & 0x20){
					bouton = four;
				}
	
		}
		if (PINC & 0x08){
			DDRC |= 0x1C;
			PORTC &= 0x08;
				if (PINC & 0x80){
					bouton = e; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = v;
				}
				else if (PINC & 0x20){
					bouton = two;
				}
	
		}
		if (PINC & 0x10){
			DDRC |= 0x1C;
			PORTC &= 0x10;
				if (PINC & 0x80){
					bouton = i; // ajouter quoi faire
				}
				else if (PINC & 0x40){
					bouton = r;
				}
				else if (PINC & 0x20){
					bouton = one;
				}
	
		}
}


int main()
{
    
    return 0;
}
