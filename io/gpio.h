/******************** DESCRIPTION ********************
This library tries to redefine the way a perticular pin 
of avr is made as output or input or made high or
low for that matter.

USER FUNCTIONS:
	initGPIO();  => initializes the initial conditions needed
	outHigh(A3); => will make PA3 high output
	outLow(A2);  => will make PA2 as low output
	inHigh(P3);  => will make PA3 as input with internal pullup on
	inLow(A4);   => This is the prestine condition for any AVR pin
	getInput(A5);=> This will give current status on PA5 (made as input) 
					'Zero' means low and 'non-zero' means high
NOTE:
	note that the arguments of the functions mention above are 
	not PA2, PA2.. and so on. Instead the arguments are
	A2, A3,...  So please use these with these functions.
	
----------------------------------------------------*/





// user config 
#define FCPU 16  // define clock freq in Mhz


// user defines functions starts 

#define GPIO 1 

#define set(reg, bit) reg|=(1<<bit)

#define clear(reg, bit) reg &= ~(1<<bit)

#define isSet(reg, bit) (reg & (1<<bit))

#define isClear(reg, bit) !(reg & (1<<bit))

uint8_t hByte( uint16_t data ){
	return ( data >> 8 );
}

uint8_t lByte( uint16_t data ){
	return ( data & 0xff );
}


// This function tries to compensate for the fact that 
// _delay_us() needs an argument value known at compile 
// time. 
void soft_delay_us(uint16_t n) {
   //n = ((uint32_t)n * 205) >> 8;
   while (n--) {
      _delay_us(1);
   }
}



/************************** REDEFINE GPIOS **************************/
#define A0 100
#define A1 101
#define A2 102
#define A3 103
#define A4 104
#define A5 105
#define A6 106
#define A7 107

#define B0 108
#define B1 109
#define B2 110
#define B3 111
#define B4 112
#define B5 113
#define B6 114
#define B7 115

#define C0 116
#define C1 117
#define C2 118
#define C3 119
#define C4 120
#define C5 121
#define C6 122
#define C7 123

#define D0 124
#define D1 125
#define D2 126
#define D3 127
#define D4 128
#define D5 129
#define D6 130
#define D7 131

#define DDRx 1
#define PORTx 2
#define PINx 3


struct io{
	volatile uint8_t* ddra;
	volatile uint8_t* porta;
	volatile uint8_t* pina;

	volatile uint8_t* ddrb;
	volatile uint8_t* portb;
	volatile uint8_t* pinb;

	volatile uint8_t* ddrc;
	volatile uint8_t* portc;
	volatile uint8_t* pinc;

	volatile uint8_t* ddrd;
	volatile uint8_t* portd;
	volatile uint8_t* pind;

}io;

void initGPIO() {
	io.ddra = &DDRA;
	io.porta = &PORTA;
	io.pina = &PINA;

	io.ddrb = &DDRB;
	io.portb = &PORTB;
	io.pinb = &PINB;

	io.ddrc = &DDRC;
	io.portc = &PORTC;
	io.pinc = &PINC;

	io.ddrd = &DDRD;
	io.portd = &PORTD;
	io.pind = &PIND;
}


void outHigh(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		*io.ddra |= (1<<(pos-100));
		*io.porta |= (1<<(pos-100));
		return;
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		*io.ddrb |= (1<<(pos-108));
		*io.portb |= (1<<(pos-108));
		return;
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		*io.ddrc |= (1<<(pos-116));
		*io.portc |= (1<<(pos-116));
		return;
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		*io.ddrd |= (1<<(pos-124));
		*io.portd |= (1<<(pos-124));
		return;
	}
}

void outLow(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		*io.ddra |= (1<<(pos-100));
		*io.porta &= ~(1<<(pos-100));
		return;
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		*io.ddrb |= (1<<(pos-108));
		*io.portb &= ~(1<<(pos-108));
		return;
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		*io.ddrc |= (1<<(pos-116));
		*io.portc &= ~(1<<(pos-116));
		return;
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		*io.ddrd |= (1<<(pos-124));
		*io.portd &= ~(1<<(pos-124));
		return;
	}
}


// toggles the internal pull up (PORTx). dosent alter DDRx.
void toggle(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		*io.porta ^= (1<<(pos-100));
		return;
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		*io.portb ^= (1<<(pos-108));
		return;
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		*io.portc ^= (1<<(pos-116));
		return;
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		*io.portd ^= (1<<(pos-124));
		return;
	}
}

void inHigh(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		*io.ddra &= ~(1<<(pos-100));
		*io.porta |= (1<<(pos-100));
		return;
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		*io.ddrb &= ~(1<<(pos-108));
		*io.portb |= (1<<(pos-108));
		return;
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		*io.ddrc &= ~(1<<(pos-116));
		*io.portc |= (1<<(pos-116));
		return;
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		*io.ddrd &= ~(1<<(pos-124));
		*io.portd |= (1<<(pos-124));
		return;
	}
}

void inLow(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		*io.ddra &= ~(1<<(pos-100));
		*io.porta &= ~(1<<(pos-100));
		return;
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		*io.ddrb &= ~(1<<(pos-108));
		*io.portb &= ~(1<<(pos-108));
		return;
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		*io.ddrc &= ~(1<<(pos-116));
		*io.portc &= ~(1<<(pos-116));
		return;
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		*io.ddrd &= ~(1<<(pos-124));
		*io.portd &= ~(1<<(pos-124));
		return;
	}
}

uint8_t getInput(uint8_t pos) {
	// check id "A"
	if(pos >= 100 && pos <= 107) {
		return (*io.pina & (1<<(pos-100)));
	}

	// is set B
	if(pos >= 108 && pos <= 115) {
		return (*io.pinb & (1<<(pos-108)));
	}

	// if set C
	if(pos >= 116 && pos <= 123) {
		return (*io.pinc & (1<<(pos-116)));
	}

	//if set D
	if(pos >= 124 && pos <= 131) {
		return (*io.pind & (1<<(pos-124)));
	}
	return 0;
}


// misc
// This function translates A1, A2 ....  to PA1, PA2 
// if the input is A3 then this function putputs PA3, DDRA, PORTA, PINA :)
volatile uint8_t* getOriginalGPIO(uint8_t pos, uint8_t type) {
	if(pos >= 100 && pos <= 107) {
		if(type == DDRx)
			return &DDRA;
		if(type == PORTx)
			return &PORTA;
		if(type == PINx)
			return &PINA;
	}
	if(pos >= 108 && pos <= 115) {
		if(type == DDRx)
			return &DDRB;
		if(type == PORTx)
			return &PORTB;
		if(type == PINx)
			return &PINB;
	}
	if(pos >= 116 && pos <= 123) {
		if(type == DDRx)
			return &DDRC;
		if(type == PORTx)
			return &PORTC;
		if(type == PINx)
			return &PINC;
	}
	if(pos >= 124 && pos <= 131) {
		if(type == DDRx)
			return &DDRD;
		if(type == PORTx)
			return &PORTD;
		if(type == PINx)
			return &PIND;
	}
	return 0;	
}


uint8_t getOriginalPos(uint8_t pos) {
	if(pos >= 100 && pos <= 107) {
		return pos-100;
	}
	if(pos >= 108 && pos <= 115) {
		return pos-108;
	}
	if(pos >= 116 && pos <= 123) {
		return pos-116;
	}
	if(pos >= 124 && pos <= 131) {
		return pos-124;
	}
	return 0;	
}





/*----------------------------------------------------------------*/


/*************************** EXAMPLE CODE ***************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/io/gpio.h"


int main() {
	initGPIO();
 
  	outHigh(A2);
    outLow(A2);
    inHigh(A3);
    inLow(A4);
    outHigh(A7);
    
    if(getInput(A7)) {
      outHigh(B3);
    }
    else {
      outHigh(B4);
    }
  	
  	while(1) { 
    
   	}
}

-------------------------------------------------------------------*/







