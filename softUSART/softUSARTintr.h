/********************* DESCRIPTION *********************
This library is an extended virsion of softUSART.h with added feature
i.e. Interrupt driven RX pin.

DEPENDENCY: 
	1. softUSART.h
	( & only one of the followings)
	2. int0.h
	3. int1.h
	4. int2.h

USER FUNCTIONS:
	1. initSoftUSARTintr(&usart0) 		// Initializes usart0 module 
	2. softUSARTWrite(&usart0, 'a') 	// to send byte (e.g. 'a') through TX of usart0
	3. softUsartData.receivedByte 		// holds the recently received byte
	4. softUsartData.justReceivedFlag 	// this flag sets when softUsartData.receivedByte is updated with new byte 

NOTE:
	1. if "RX_CALLBACK" is set to "YES"  the the programmer has to define "softRX_callback()" else the program WILL NOT COMPILE.
	2. Connect RX wire to appropriate INTx PIN.
	5. Baud rate defined in softUSART.h
	4. the received byte will not be updated within object property, 
	   rather will be updated in "softUsartData.receivedByte"	for
	   all virtual usart modules
 ------------------------------------------------------*/



/*********************** INTERNAL ***********************/
#define INTR0 1
#define INTR1 2
#define INTR2 3
#define YES 1
#define NO 2
/*------------------------------------------------------*/



/******************* USER CONFIGURABLE *******************/
#define INTR_PIN_RX INTR0 // possible options are INTR0(PD2), INTR1(PD3), INTR2(PB2)
#define RX_CALLBACK YES // possiable options YES, NO
/*-------------------------------------------------------*/


/******************* INCLUDE DEPENDENCY *******************/
#include "softUSART.h"

#undef RX_DDR
#undef RX_PORT
#undef RX_PIN
#undef RX_POS

#if INTR_PIN_RX == INTR0
	#include "../int/intr/int0.h"	
	#define RX_DDR DDRD
	#define RX_PORT PORTD
	#define RX_PIN PIND
	#define RX_POS PD2

#elif INTR_PIN_RX == INTR1
	#include "../int/intr/int1.h"
	#define RX_DDR DDRD
	#define RX_PORT PORTD
	#define RX_PIN PIND
	#define RX_POS PD3

#elif INTR_PIN_RX == INTR2
	#include "../int/intr/int2.h"
	#define RX_DDR DDRB
	#define RX_PORT PORTB
	#define RX_PIN PINB
	#define RX_POS PB2

#endif
/*--------------------------------------------------------*/



/******************** GOBAL VARIABLES ********************/
struct softUsartIntr {
	uint8_t receivedByte;
	uint8_t justReceivedFlag;
} softUsartData; 
/*-------------------------------------------------------*/




/********************* USER FUNCTIONS *********************/
void initSoftUSARTintr(struct softUsart *usart){
	usart->receivedByte = 0;
	inHigh(usart->rx);  // active low bus
	outHigh(usart->tx); // idle state is high
	cli();
	#if INTR_PIN_RX == INTR0
		// interrupt @ falling edge
		DDRD &= ~(1<<PD2);
		PORTD |=(1<<PD2); 
   		MCUCR |= (2<<0); 
   		GICR |= (1<<INT0);
	#elif INTR_PIN_RX == INTR1
		// interrupt @ falling edge
		DDRD &= ~(1<<PD3);
		PORTD |=(1<<PD3); 
   		MCUCR |= (1<<ISC11);  
   		GICR |= (1<<INT1);
	#elif INTR_PIN_RX == INTR2
   		// interrupt @ falling edge
		DDRB &= ~(1<<PB2);
		PORTB |=(1<<PB2); // connect internal pull up
   		MCUCSR &= ~(1<<ISC2);
   		GICR |= (1<<INT2);
	#endif
	sei();
}
/*--------------------------------------------------------*/




/*********************** INTERNALS ***********************/
void softRX_callback(void); //	function prototype

#if INTR_PIN_RX == INTR0
	void int0_Callback() {
		uint8_t byte=0x00;
		uint8_t i;
	 
		_delay_us(BAUD_DELAY>>2); //.5 delay; go to the middle of start bit.
		// sample the following 8 data bits
		for(i=0; i<8; i++) {
			_delay_us(BAUD_DELAY);
			// sample the first bit
			if(RX_PIN & (1<<RX_POS)) {
				byte |= (1<<i);
			}	
		}
		softUsartData.receivedByte = byte;
		softUsartData.justReceivedFlag = 1;
		#if RX_CALLBACK == YES
			softRX_callback();
		#endif
	}
#elif INTR_PIN_RX == INTR1
	void int1_Callback() {
		uint8_t byte=0x00;
		uint8_t i;
	 
		_delay_us(BAUD_DELAY>>2); //.5 delay; go to the middle of start bit.
		// sample the following 8 data bits
		for(i=0; i<8; i++) {
			_delay_us(BAUD_DELAY);
			// sample the first bit
			if(RX_PIN & (1<<RX_POS)) {
				byte |= (1<<i);
			}	
		}
		softUsartData.receivedByte = byte;
		softUsartData.justReceivedFlag = 1;
		#if RX_CALLBACK == YES
			softRX_callback();
		#endif
	}
#elif INTR_PIN_RX == INTR2
	void int2_Callback() {
		uint8_t byte=0x00;
		uint8_t i;
	 
		_delay_us(BAUD_DELAY>>2); //.5 delay; go to the middle of start bit.
		// sample the following 8 data bits
		for(i=0; i<8; i++) {
			_delay_us(BAUD_DELAY);
			// sample the first bit
			if(RX_PIN & (1<<RX_POS)) {
				byte |= (1<<i);
			}	
		}
		softUsartData.receivedByte = byte;
		softUsartData.justReceivedFlag = 1;
		#if RX_CALLBACK == YES
			softRX_callback();
		#endif
	}
#endif
/*--------------------------------------------------------*/



/************************ UNDEFINE ************************/
#undef INTR0
#undef INTR1
#undef INTR2
#undef YES
#undef NO

#undef INTR_PIN_RX
#undef RX_CALLBACK

#undef RX_DDR
#undef RX_PORT
#undef RX_PIN
#undef RX_POS
/*--------------------------------------------------------*/





/********************** EXAMPLE CODE **********************
#include <avr/io.h>
#include <util/delay.h>
#include "mega16/softUSART/softUSARTintr.h"
#include "mega16/ext/lcd16x2/lcd.h"

struct softUsart usart0;

int main() {
    initGPIO();
    
    usart0.rx = D2;
    usart0.tx = A1;

    initSoftUSARTintr(&usart0);

    LCDInit(LS_NONE);
    LCDClear();
    _delay_ms(100);
   
    while(1) {
        _delay_ms(500);
        LCDWriteIntXY(0,0,softUsartData.receivedByte,3);
        _delay_ms(100);
    }
}


void softRX_callback() {
    softUSARTWrite(&usart0, softUsartData.receivedByte+1);
}
---------------------------------------------------------*/
