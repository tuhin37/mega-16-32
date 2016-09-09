/******************* DEPENDENCY *******************

1. ShiftRegHC595.h

---------------------------------------------------/




/****************** Description *******************
This Library is build over the basic Shift Register library "ShiftRegHC595.h" with some added extended features
USE: use this library instead of the basic one  when you need to connect more than one shift registers in one MCU
and you dont want to cascade (daisy chain) them.  

NOTE: The shift registers share same "shift clk" and "latch clk" lines. just data lines are different. For this you 
have to write data to all the shift registers simultaniously. Also the first Oth SR is already defined in "ShiftRegHC595.h"
if you wish to alter the connections for the 0th SR then do it in the user configurable area of  "ShiftRegHC595.h"


----SEPARATE DATA LINES AND COMMON CLOCK LINES, NO DAISY----

***************************************************/

// Dependency check
#ifndef HC595_DS_POS
	#include "ShiftRegHC595.h"
#endif



/***************** user configureable area *****************/

// If more than one shift registers are connected (NOT CASCADED)
// They can and will share same "shift clk" and "latch clock" lines
// But serial data lines will differ. i.e. DS1, DS2, DS3 etc.
// In such case specify the aditional data lines here.


// DS1 PIN CONNECTED TO
#define HC595_DS1_DDR 	DDRB
#define HC595_DS1_PORT 	PORTB
#define HC595_DS1_POS	PB3


// // DS2 PIN CONNECTED TO
// #define HC595_DS2_DDR 	DDRB
// #define HC595_DS2_PORT 	PORTB
// #define HC595_DS2_POS	PB4



// ... you can go on and continue to add more shift registers if needed`

/****************** user config area ends ******************/


// shift pulse function for 2nd SR
#ifdef HC595_DS1_POS
	//Low level macros to change data (DS)lines
	#define HC595DataHigh1() (HC595_DS_PORT|=(1<<HC595_DS1_POS))
	#define HC595DataLow1() (HC595_DS_PORT&=(~(1<<HC595_DS1_POS)))
#endif

// shift pulse function for 3rd SR
#ifdef HC595_DS2_POS
	//Low level macros to change data (DS)lines
	#define HC595DataHigh2() (HC595_DS_PORT|=(1<<HC595_DS2_POS))
	#define HC595DataLow2() (HC595_DS_PORT&=(~(1<<HC595_DS2_POS)))
#endif



// if only 1 SR's in use
#if !defined(HC595_DS1_POS) && !defined(HC595_DS1_POS)
// This is the function that is going to be called by users.
void HC595WriteCK(uint8_t data) {	// ck stands for common clk
	//Send each 8 bits serially
	//Order is MSB first
	for(uint8_t i=0;i<8;i++) {
		//Output the data on DS line according to the
		//Value of MSB
		if(data & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow();
		}
		HC595Pulse();	//Pulse the Clock line
		data=data<<1;	//Now bring next bit at MSB position
	}
	//Now all 8 bits have been transfered to shift register
	//Move them to output latch at one
	HC595Latch();
}
#endif


// if 2 SR's are in use
#if defined(HC595_DS1_POS) && !defined(HC595_DS2_POS)
void HC595WriteCK(uint8_t data0, uint8_t data1) {
	// Make the new DS line as output if not already
	if(!(HC595_DS1_DDR & (1<<HC595_DS1_POS)))
		HC595_DS1_DDR |= (1<<HC595_DS1_POS);
	//Send each 8 bits serially
	//Order is MSB first
	for(uint8_t i=0;i<8;i++) {
		//Output the data on DS line according to the
		//Value of MSB
		// for SR0
		if(data0 & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow();
		}

		// for SR1
		if(data1 & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh1();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow1();
		}

		HC595Pulse();	//Pulse the Clock line
		data0=data0<<1;	//Now bring next bit at MSB position
		data1=data1<<1;	//Now bring next bit at MSB position
	}
	//Now all 8 bits have been transfered to shift register
	//Move them to output latch at one
	HC595Latch();
}
#endif

// if 3 SR's are in use
#if defined(HC595_DS1_POS) && defined(HC595_DS2_POS)
void HC595WriteCK(uint8_t data0, uint8_t data1, uint8_t data2) {
	// Make the new DS lines as output if not already
	if(!(HC595_DS1_DDR & (1<<HC595_DS1_POS)))
		HC595_DS1_DDR |= (1<<HC595_DS1_POS);
	if(!(HC595_DS2_DDR & (1<<HC595_DS2_POS)))
		HC595_DS2_DDR |= (1<<HC595_DS2_POS);
	//Send each 8 bits serially
	//Order is MSB first
	for(uint8_t i=0;i<8;i++) {
		//Output the data on DS line according to the
		//Value of MSB
		// for SR0
		if(data0 & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow();
		}

		// for SR1
		if(data1 & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh1();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow1();
		}

		// for SR2
		if(data2 & 0b10000000) {
			//MSB is 1 so output high
			HC595DataHigh2();
		}	
		else {
			//MSB is 0 so output high
			HC595DataLow2();
		}

		HC595Pulse();	//Pulse the Clock line
		data0=data0<<1;	//Now bring next bit at MSB position
		data1=data1<<1;	//Now bring next bit at MSB position
		data2=data2<<1; //Now bring next bit at MSB position
	}
	//Now all 8 bits have been transfered to shift register
	//Move them to output latch at one
	HC595Latch();
}
#endif


// ... you can go on and continue to add more shift registers


/**************************** Basic Functions End ****************************/




/******************************** Example Code ********************************/
/*


#include <avr/io.h>
#include <util/delay.h>

#include "ShiftRegHC595_comClk.h"



void main(){
	//Initialize HC595 system
	HC595Init();	
	// first byte goes to SR with DS0 line
	HC595WriteCK(0b10101010, 0b01010101);
	while(1) {
	}
}

*/

