/***************** Description *******************
This is the basic library for Shift Register
(reffered to as SR from now on) 74HC595.
Its a 8 bit SR. It requires 3 MCU lines and it can drive
8 different loads individually.

NOTE: This library lets you connect one SR with the system. But there are situations 
when you require to add more than one SR. In that case use the extended libraries, along with this

EXTENDED LIBRARIES:
1. shiftreghc595_comclk.h
2. shiftreghc595_daisy.h
3. shiftreghc595_daisy.h

----74HC595 SR BASE LIBRARY----

***************************************************/





/******************************* Connections *******************************/
/*	HC595 Shift register needs Three MCU lines to work and GND, VCC for power
	
	DS (Serial data PIN-14) is connected to serial data output pin of MCU
	SH_CP (shift register clk input PIN-11) data bit on DS is captured in shift registers LSB when a rising edge occurs in this line
	ST_CP (Storage Register clk input PIN-12) at rising clk the 8 bits of shift register comes to the latch buffer and becomes available as output.

	VCC (PIN-16)
	GND (PIN-8)
	OE` (PIN-13) make  gnd
	MR` ( Master Reset active low PIN-10). Usually pulled up. if connected to gnd the latch resets  
 */




/***************** user configureable area *****************/
// DS PIN CONNECTED TO
#define HC595_DS_DDR 	DDRB
#define HC595_DS_PORT 	PORTB
#define HC595_DS_POS	PB0

// SH_CP (Shift Clk) PIN CONNECTED TO
#define HC595_SHCP_DDR	DDRB
#define HC595_SHCP_PORT	PORTB
#define HC595_SHCP_POS 	PB2

// ST_CP (Storage/Latch Clk) PIN CONNECTED TO
#define HC595_STCP_DDR	DDRB
#define HC595_STCP_PORT	PORTB
#define HC595_STCP_POS 	PB1

/****************** user config area ends ******************/


/****************************** DEPENDENCY ******************************/
#ifndef GPIO
	#include "../../io/gpio.h"
#endif
/*----------------------------------------------------------------------*/


/************************* Basic Functions start *************************/

// global Variable for advanced bitwise operation
volatile uint8_t SR0; // holds the current byte loaded into SR0


//Initialize HC595 Shift Register
void HC595Init() {
	HC595_DS_DDR |= (1<<HC595_DS_POS);		// Make DS output
	HC595_SHCP_DDR |= (1<<HC595_SHCP_POS); 	// Make SHCP output
	HC595_STCP_DDR |= (1<<HC595_STCP_POS );	// Make STCP output
}


//Low level macros to change data (DS)lines
#define HC595DataHigh() (HC595_DS_PORT|=(1<<HC595_DS_POS))
#define HC595DataLow() (HC595_DS_PORT&=(~(1<<HC595_DS_POS)))


//Sends a single clk pulse (rising edge) on SH_CP (shift Clk) line 
void HC595Pulse() {
	HC595_SHCP_PORT |= (1<<HC595_SHCP_POS);//HIGH
	HC595_SHCP_PORT &= (~(1<<HC595_SHCP_POS));//LOW

}

//Sends a single clk pulse (rising edge) on ST_CP (Storage/latch Clk) line 
void HC595Latch() {
	//Pulse the Latch Clock
	HC595_STCP_PORT|=(1<<HC595_STCP_POS);//HIGH
	_delay_loop_1(1);
	HC595_STCP_PORT&=(~(1<<HC595_STCP_POS));//LOW
	_delay_loop_1(1);
}



// This is the function that is going to be called by users.
void HC595Write(uint8_t data) {
	
	// first update the global variable that holds the current loaded in SR0
	SR0 = data;
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



// these two function  sets or clears a specific bit position at SR output
// thus making is easy to turn on and off any perticular load connected to 
// SR without affecting the other loaded
void HC595WriteSetBit(uint8_t bitPos) {
	uint8_t newByte = SR0 | (1<<bitPos);
	HC595Write(newByte);
}

void HC595WriteClearBit(uint8_t bitPos) {
	uint8_t newByte = SR0 & ~(1<<bitPos);
	HC595Write(newByte);
}





/**************************** Basic Functions End ****************************/





/******************************** Example Code *******************************
#include <avr/io.h>
#include <util/delay.h>
#include "ShiftRegHC595.h"

void main(){
	
	//Initialize HC595 system
	HC595Init();
	while(1) {
		HC595Write(0b00110101);	//Write the data to HC595
		_delay_ms(500);						//Wait
		HC595Write(0b11001010);
		_delay_ms(500); 		
	}
}


----------------------------------------------------------------------------*/
