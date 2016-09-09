/******************* DEPENDENCY *******************

1. ShiftRegHC595.h

---------------------------------------------------/



/****************** Description *******************
This Library is build over the basic Shift Register library "ShiftRegHC595.h" with some added extended features
USE: use this library instead of the basic one  when you need to connect more than one shift registers in one MCU
and you dont want to cascade (daisy chain) them.

Each SR will need 3 different MCU lines. 

NOTE: The shift registers do not share any lines with each other
so you can operate them separately without having to 
update the others. IFF you want to reduce number of MCU lines you 
can always make the latch pulse common betwen them. That dosent effect the 
other registers in the same latch clk line. Also the first Oth SR is already defined in "ShiftRegHC595.h"
if you wish to alter the connections for the 0th SR then do it in the user configurable area of  "ShiftRegHC595.h"


----FULLY SEPERATED SR MODULES 3 LINES PER SR ----

***************************************************/

// Dependency check
#ifndef HC595_DS_POS
	#include "ShiftRegHC595.h"
#endif



/***************** user configureable area *****************/

// 2nd SR-----------------------------
//DS (Serial data) PIN-14 CONNECTED TO
#define HC595_DS1_DDR 	DDRA
#define HC595_DS1_PORT 	PORTA
#define HC595_DS1_POS	PA4

// SH_CP (Shift Clk) PIN-11 CONNECTED TO
#define HC595_SHCP1_DDR		DDRA
#define HC595_SHCP1_PORT	PORTA
#define HC595_SHCP1_POS 	PA5

// ST_CP (Storage/Latch Clk) PIN-12 CONNECTED TO
#define HC595_STCP1_DDR		DDRA
#define HC595_STCP1_PORT	PORTA
#define HC595_STCP1_POS 	PA2


// // 3rd SR----------------------------
// #define HC595_DS2_DDR 	DDRB
// #define HC595_DS2_PORT 	PORTB
// #define HC595_DS2_POS	PB3

// // SH_CP (Shift Clk) PIN CONNECTED TO
// #define HC595_SHCP2_DDR		DDRB
// #define HC595_SHCP2_PORT	PORTB
// #define HC595_SHCP2_POS 	PB4

// // ST_CP (Storage/Latch Clk) PIN CONNECTED TO
// #define HC595_STCP2_DDR		DDRB
// #define HC595_STCP2_PORT	PORTB
// #define HC595_STCP2_POS 	PB5


// ... you can go on and continue to add more shift registers if needed

/****************** user config area ends ******************/


#ifdef HC595_SHCP1_POS
	// global section
	volatile uint8_t SR1;

	void HC595Init1() {
		HC595_DS1_DDR |= (1<<HC595_DS1_POS);		// Make DS output
		HC595_SHCP1_DDR |= (1<<HC595_SHCP1_POS); 	// Make SHCP output
		HC595_STCP1_DDR |= (1<<HC595_STCP1_POS );	// Make STCP output
	}

	//Low level macros to change data (DS)lines
	#define HC595DataHigh1() (HC595_DS1_PORT|=(1<<HC595_DS1_POS))
	#define HC595DataLow1() (HC595_DS1_PORT&=(~(1<<HC595_DS1_POS)))

	void HC595Pulse1() {
		HC595_SHCP1_PORT |= (1<<HC595_SHCP1_POS);//HIGH
		HC595_SHCP1_PORT &= (~(1<<HC595_SHCP1_POS));//LOW
	}

	void HC595Latch1() {
		//Pulse the Latch Clock
		HC595_STCP1_PORT|=(1<<HC595_STCP1_POS);//HIGH
		_delay_loop_1(1);
		HC595_STCP1_PORT&=(~(1<<HC595_STCP1_POS));//LOW
		_delay_loop_1(1);
	}

	// This is the function that is going to be called by users.
	void HC595Write1(uint8_t data) {
		// update the current byte
		SR1 = data;
		//Send each 8 bits serially
		//Order is MSB first
		for(uint8_t i=0;i<8;i++) {
			//Output the data on DS line according to the
			//Value of MSB
			if(data & 0b10000000) {
				//MSB is 1 so output high
				HC595DataHigh1();
			}
			else {
				//MSB is 0 so output high
				HC595DataLow1();
			}
			HC595Pulse1();	//Pulse the Clock line
			data=data<<1;	//Now bring next bit at MSB position
			
		}
		//Now all 8 bits have been transfered to shift register
		//Move them to output latch at one
		HC595Latch1();
	}

	// these two function  sets or clears a specific bit position at SR output
	// thus making is easy to turn on and off any perticular load connected to 
	// SR without affecting the other loaded
	void HC595Write1SetBit(uint8_t bitPos) {
		uint8_t newByte = SR1 | (1<<bitPos);
		HC595Write1(newByte);
	}

	void HC595Write1ClearBit(uint8_t bitPos) {
		uint8_t newByte = SR1 & ~(1<<bitPos);
		HC595Write1(newByte);
	}
#endif

#ifdef HC595_SHCP2_POS
	// global section
	volatile uint8_t SR2;
	
	void HC595Init2() {
		HC595_DS2_DDR |= (1<<HC595_DS2_POS);		// Make DS output
		HC595_SHCP2_DDR |= (1<<HC595_SHCP2_POS); 	// Make SHCP output
		HC595_STCP2_DDR |= (1<<HC595_STCP2_POS );	// Make STCP output
	}

	//Low level macros to change data (DS)lines	
	#define HC595DataHigh2() (HC595_DS2_PORT|=(1<<HC595_DS2_POS))
	#define HC595DataLow2() (HC595_DS2_PORT&=(~(1<<HC595_DS2_POS)))
	
	void HC595Pulse2() {
		HC595_SHCP2_PORT |= (1<<HC595_SHCP2_POS);//HIGH
		HC595_SHCP2_PORT &= (~(1<<HC595_SHCP2_POS));//LOW
	}

	void HC595Latch2() {
		//Pulse the Latch Clock
		HC595_STCP2_PORT|=(1<<HC595_STCP2_POS);//HIGH
		_delay_loop_1(1);
		HC595_STCP2_PORT&=(~(1<<HC595_STCP2_POS));//LOW
		_delay_loop_1(1);
	}

	// This is the function that is going to be called by users.
	void HC595Write2(uint8_t data) {
		//Send each 8 bits serially
		//Order is MSB first
		for(uint8_t i=0;i<8;i++) {
			//Output the data on DS line according to the
			//Value of MSB
			if(data & 0b10000000) {
				//MSB is 1 so output high
				HC595DataHigh2();
			}
			else {
				//MSB is 0 so output high
				HC595DataLow2();
			}
			HC595Pulse2();	//Pulse the Clock line
			data=data<<1;	//Now bring next bit at MSB position
			
		}
		//Now all 8 bits have been transfered to shift register
		//Move them to output latch at one
		HC595Latch2();
	}

	void HC595Write2SetBit(uint8_t bitPos) {
		uint8_t newByte = SR2 | (1<<bitPos);
		HC595Write2(newByte);
	}

	void HC595Write2ClearBit(uint8_t bitPos) {
		uint8_t newByte = SR2 & ~(1<<bitPos);
		HC595Write2(newByte);
	}
#endif



// ... you can go on and continue to add more shift registers


/**************************** Basic Functions End ****************************/




/******************************** Example Code ********************************/
/*


#include <avr/io.h>
#include <util/delay.h>

#include "ShiftRegHC595_X.h"



void main(){
	//Initialize HC595 system
	HC595Init();
	HC595Init1();
	
	HC595Write(0b10101010);
	HC595Write1(0b11001100);
	while(1) {
	}
}


*/

