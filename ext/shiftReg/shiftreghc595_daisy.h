/******************* DEPENDENCY *******************

1. ShiftRegHC595.h

---------------------------------------------------/



/****************** Description *******************
This Library is build over the basic Shift Register library "ShiftRegHC595.h" with some added extended features
USE: use this library instead of the basic one  when you need to connect more than one shift registers in one MCU
in cascade (daisy chain) 

The shift registers share same "shift clk" and "latch clk" lines. just data line(DS) of next SR comes from 
Q7S(PIN-9) of previous SR. So essentially 3 lines from MCU is used all the time. Depending on the number of SR's 
cascaded output lines can be as many as you want.

Higher byte(15th bit to 8th bit) of input data goes to Cascaded Shift register (Far from MCU) Q7 => MSB(15th bit)
lower byte (7th bit to 0th bit) of input data goes to first Shift register(closest to MCU) Q7 => 7th bit 

NOTE: The first Oth SR is already defined in "ShiftRegHC595.h"
if you wish to alter the connections for the 0th SR then do it in the user configurable area of  "ShiftRegHC595.h"

----DAISY CHAIN----*/



// Dependency check
#ifndef HC595_DS_POS
	#include "ShiftRegHC595.h"
#endif





/***************** user configureable area *****************/

// if more than one 75HC595 shift registers 
//are cascaded then spicify number of cascaded shift registers( total shift registers - 1 )
#define DAISY_CHAIN 1	// options 1, 2, 3. (0 => no daisy chain)

/****************** user config area ends ******************/




// writes double word
// 2 shift registers (Total 16 pins)
// SR0(closer to MCU) and SR1(far from MCU)
#if DAISY_CHAIN == 1
	void HC595WriteDW(uint8_t data0, uint8_t data1) {
		HC595Write(data1);
		HC595Write(data0);
	}
#endif

// writes Triple words
// 3 shift registers (Total 24 pins)
// SR0(Closest to MCU); SR1(middle); SR2(Farthest)
#if DAISY_CHAIN == 2
	void HC595WriteTW(uint8_t data0, uint8_t data1, uint8_t data2) {
		HC595Write(data2);
		HC595Write(data1);
		HC595Write(data0);
	}
#endif

// Writes Quadruple words
// 4 shift registers (Total 32 pins)
// SR0(Closest to MCU); SR1(middle); SR2(middle2); SR3(Farthest)
#if DAISY_CHAIN == 2
	void HC595WriteQW(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3) {
		HC595Write(data3);
		HC595Write(data2);
		HC595Write(data1);
		HC595Write(data0);
	}
#endif


/******************************** Example Code ********************************/
/*

#include <avr/io.h>
#include <util/delay.h>

#include "ShiftRegHC595_daisy.h"



void main(){
	//Initialize HC595 system
	HC595Init();	
	// first byte goes to SR closest to MCU
	HC595WriteDW(0b10101010, 0b01010101);
	while(1) {
	}
}

			

*/
