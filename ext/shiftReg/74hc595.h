/****************************** DESCRIPTION *****************************
This library helps in interfacing shift registers 74HC595 with MCU.


CONNECTION:
	HC595 Shift register needs Three MCU lines to work and GND, VCC for power
	
	DS (Serial data PIN-14) is connected to serial data output pin of MCU
	SH_CP (shift register clk input PIN-11) data bit on DS is captured in shift registers LSB when a rising edge occurs in this line
	ST_CP (Storage Register clk input PIN-12) at rising clk the 8 bits of shift register comes to the latch buffer and becomes available as output.

	VCC (PIN-16)
	GND (PIN-8)
	OE` (PIN-13) make  gnd
	MR` ( Master Reset active low PIN-10). Usually pulled up. if connected to gnd the latch resets  

USER FUNCTIONS: 
	HC595Write(&sr0, byte)	=> writes byte into sr0
	HC595setBit(&sr0, 5)	=> sets 5th(starting from 0) bit in sr0;
	HC595clearBit(&sr0, 4)	=> clears 4th(starting from 0) bit in sr0;


USAGE:
	Programmers need to declear shift register objects of type "shiftReg" in global section.
	E.g. "shiftReg  sr0;". There can be multiple shift registers connected to one single MCU
	they can be sr1, sr2, ... etc
	Each of these objects will inherit the following properties
		sr0.ds 		-> serial data line (PIN-14 of 74HC595)
		sr0.stcp 	-> storage / latch clock (PIN-12)
		sr0.shcp 	-> shift clock (PIN-11)
		sr0.data	-> current byte present in that perticular shift register

	When two shift registers share same latch/storage clock lines to save gpio pins used,
	the programmer need to set that up like this.
		sr0.stcp = A0;
		sr1.stcp = A0;

	When Daisy Chaining, MCU will see it as one sr only. programmer will have to call
	"HC595Write()" multiple times (n times if n shift registers are daisy chained together) consiqutively  
	like this: (assuming N shift Regiters are daisy chained togeteher)
		HC595Write(&sr0, byteN); 	=>	Nth Shift Regiter is the farthest from MCU 
		HC595Write(&sr0, byteN-1);
		HC595Write(&sr0, byteN-2);
		.
		.
		.
		HC595Write(&sr0, byte1);	=> Closest to MCU

NOTE:
	Initialize initGPIO(); before using any of this library functions.	

--------------------------------------------------------------------------------------------------*/

/***************************** DEPENDENCY ******************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif
/*----------------------------------------------------------------------*/




/******************************** GLOBAL ********************************/
typedef struct {
	volatile uint8_t ds;
	volatile uint8_t shcp;
	volatile uint8_t stcp;
	volatile uint8_t data;
}shiftReg;
/*---------------------------------------------------------------------*/



/**************************** USER FUNCTIONS ****************************/
// This is the function that is going to be called by users.
void HC595Write( shiftReg *sr, uint8_t data) {
	// first update the global variable that holds the current loaded in SR0
	sr->data = data;
	//Send each 8 bits serially
	//Order is MSB first
	for(uint8_t i=0;i<8;i++) {
		//Output the data on DS line according to the
		//Value of MSB
		if(data & 0b10000000) {
			//MSB is 1 so output high
			outHigh(sr->ds);
		}
		else {
			//MSB is 0 so output high
			outLow(sr->ds);
		}
		//Pulse the shift Clock line 
		outHigh(sr->shcp);
		outLow(sr->shcp);	
		data=data<<1;	//Now bring next bit at MSB position
	}
	//Now all 8 bits have been transfered to shift register
	
	// Move them to output latch at onece
	// latch to output (generate a latch pulse)
	outHigh(sr->stcp);
	//_delay_loop_1(1);
	outLow(sr->stcp);
	//_delay_loop_1(1);
}


void HC595setBit( shiftReg *sr, uint8_t pos) {
	uint8_t newByte=0;
	newByte = (sr->data) | (1<<pos);
	HC595Write(sr, newByte);
	sr->data = newByte;
}


void HC595clearBit( shiftReg *sr, uint8_t pos) {
	uint8_t newByte=0;
	newByte = (sr->data) & ~(1<<pos);
	HC595Write(sr, newByte);
	sr->data = newByte;
}
/*------------------------------------------------------------*/


/************************ EXAMPLE CODE ************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "mega16/io/gpio.h"
#include "mega16/ext/shiftReg/74hc595.h"


shiftReg  sr0;

int main(){
    initGPIO();
    
    sr0.ds = D0;
    sr0.stcp = D1;
    sr0.shcp = D2;
    sr0.data = 0;


    HC595Write(&sr0, 0b11001100);
    HC595clearBit(&sr0, 6);
    HC595setBit(&sr0, 0);
    
    while(1) {
    }
}
-------------------------------------------------------------*/