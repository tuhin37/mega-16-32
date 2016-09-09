/******************************** DESCRIPTION *******************************
This Library uses i2c libray and internal i2c hardware of atmega to interface 
EEPROM 24C64/24C32 with avr.

In minimum config This EEPROM requires the following connections
	1. VCC (PIN-8)
	2. GND (PIN-4)
	3. SCL (PIN-6) pulled up 
	4. SDA (PIN-5) pulled up

This config will allow only 8 KB memory space. There are Address pins 
A0(PIN1), A1(PIN2), A2(PIN3) which gives total 8 combinations. Each 
combination gives 8 KB memory space so 8 * 8 KB = 64 KB total

ALL EEPROM in this family has device address in this format 
[1 0 1 0 A2 A1 A0] A0 => LSB. If A2:0 pins are floating then 
they internally become grounded. So in that case device 
address becomes 0b1010000 <=> 80; So 
so one device can be addressed with 80...88 when A2:0 pins are in use.

GLOBAL:
	seqRead[32] => 	array will be updated with one page (32 bytes) of data
					when EEReadPage() function is called. This function accepts 
					two arguments. First one is device address and secods one 
					is page address. In one device there 256 pages(24C64) and
					128 pages(24C32).

	seqWrite[32] => when EEWritePage() function is called, It will write
					all 32 values present in in this array into EEPROM.
					It accepts same arguments as EEReadPage() function.



USER FUNCTIONS:
	1. EEWriteByte() => Writes a single byte into a specific memory location 
						of a specific device
	2. EEWritePage() =>	Writes 32 bytes at one go on to one page of a device
	3. EEReadByte()	 => Reads a byte from a specific location of a specific
						device.
	4. EEReadPage()  => Reads and entire page(32 byte) from a specific device
---------------------------------------------------------------------------*/





/********************************** GLOBAL **********************************/
volatile uint8_t seqRead[32];
volatile uint8_t seqWrite[32]="Tuhin Sengupta"; // Dummy text
/*--------------------------------------------------------------------------*/




/******************************** DEPENDENCY ********************************/
#ifndef I2C
	#include "../../int/i2c/i2c.h"
#endif
/*--------------------------------------------------------------------------*/




/****************************** USER FUNCTIONS ******************************/
uint8_t EEWriteByte(uint8_t devAddress, uint16_t memAddress, uint8_t data) {
	
	if(!TWI_START())
		return 0; //start faild

	// send SLA+W
	if(!TWI_SLA_W(devAddress))
		return 0; // SLA failed

	// send High Byte of Memory Address
	if(!TWI_ByteWrite(memAddress>>8))
		return 0;
	
	// send Low Byte of Memory Address
	if (!TWI_ByteWrite(memAddress))
		return 0;
	
	// send Byte to be stord
	if(!TWI_ByteWrite(data))
		return 0;
	
	// put a Atop Condiiton on the Bus
	TWI_STOP();
		
	// Give Time for Internal Write Cycle to complete
	_delay_ms(12);

	//Return Success
	return 1;
}



// This function takes in page address(0..255) and writes 32 bytes in one go.
//  It will write 32 bytes from seqWrite[] array
uint8_t EEWritePage(uint8_t devAddress, uint16_t pageAddress, uint8_t data) {
	uint16_t memAddress = pageAddress<<5; // *32
	uint8_t i;
	if(!TWI_START())
		return 0; //start faild

	// send SLA+W
	if(!TWI_SLA_W(devAddress))
		return 0; // SLA failed

	// send High Byte of Memory Address
	if(!TWI_ByteWrite(memAddress>>8))
		return 0;
	
	// send Low Byte of Memory Address
	if (!TWI_ByteWrite(memAddress))
		return 0;
	
	for(i=0; i<32; i++) {
		// send Byte to be stord
		if(!TWI_ByteWrite(seqWrite[i]))
			return 0;
	}
	// put a Atop Condiiton on the Bus
	TWI_STOP();
		
	// Give Time for Internal Write Cycle to complete
	_delay_ms(12);

	//Return Success
	return 1;
}



// This function will read 1 byte from device(device address)
// and from memory location recognized by memory Addres  
uint8_t EEReadByte(uint8_t devAddress, uint16_t memAddress) {
	uint8_t data;

	//Dummy Write Sequence start
	if(!TWI_START())
		return 0; //start faild

	if(!TWI_SLA_W(devAddress))
		return 0; // SLA failed
		
	// send High Byte of Memory Address
	if(!TWI_ByteWrite(memAddress>>8))
		return 0;
	
	// send Low Byte of Memory Address
	if (!TWI_ByteWrite(memAddress))
		return 0;
	//-------------------------------


	// Repear start
	if(!TWI_RESTART())
		return 0;
	
	// SLA+R 
	if(!TWI_SLA_R(devAddress))
		return 0;

	//Read the data
	data=TWI_ByteRead(0);

	TWI_STOP();
	
	return data;
}




// This function will read 32 bytes from a single page and 
// will put those 32 bytes in seqRead[] array
uint8_t EEReadPage(uint8_t devAddress, uint16_t pageAddress) {
	uint16_t memAddress = pageAddress<<5; // *32
	//Dummy Write Sequence start
	if(!TWI_START())
		return 0; //start faild

	if(!TWI_SLA_W(devAddress))
		return 0; // SLA failed
		
	// send High Byte of Memory Address
	if(!TWI_ByteWrite(memAddress>>8))
		return 0;
	
	// send Low Byte of Memory Address
	if (!TWI_ByteWrite(memAddress))
		return 0;
	//-------------------------------


	// Repear start
	if(!TWI_RESTART())
		return 0;
	
	// SLA+R 
	if(!TWI_SLA_R(devAddress))
		return 0;



	//Read the data
	if(!TWI_multiByteRead(seqRead, (sizeof(seqRead)/sizeof(uint8_t))))
		return 0;
	
	return 1; // denotes successful
}
/*------------------------------------------------------------------------*/

		



/****************************** EXAMPLE CODE (PAGE R/W)******************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/io/gpio.h"
#include "mega16/ext/lcd16x2/lcd.h"
volatile uint8_t byte;
#include "mega16/ext/eeprom/eeprom.h"



int main() {
    initGPIO();
    LCDInit(LS_NONE);
    _delay_ms(200);
    LCDClear();


    uint8_t i;

    // This will set the SCL frequency
    // Refer to SCL frequency table in i2c library folder
    initTWI(0,5);


    // this will write whatever present in seqWrite[]
    // in device 80, page number 0
    EEWritePage(80, 0, 120);    
    LCDWriteStringXY(0,0, "Writing...");
    
    
    _delay_ms(1000);
    LCDClear();
    LCDWriteStringXY(0,0, "Reading...");
    _delay_ms(1000);
    LCDClear();

    // This function call will read page 0 of device 80
    // and store 32 bytes into seqRead[]  array
    EEReadPage(80, 0);
    for (i=0; i<32; i++ ) {
        LCDWriteIntXY(0,0, seqRead[i],3);
        outHigh(A1);
        _delay_ms(500);
        outLow(A1);
        _delay_ms(500);
    }
    
   while(1);
}
------------------------------------------------------------------------*/








/****************************** EXAMPLE CODE (SINGLE BYTE R/W) ******************************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "mega16/io/gpio.h"
#include "mega16/ext/lcd16x2/lcd.h"
volatile uint8_t byte;
#include "mega16/ext/eeprom/eeprom.h"


int main() {
    initGPIO();
    LCDInit(LS_NONE);
    _delay_ms(200);
    LCDClear();


    // This will set the SCL frequency
    // Refer to SCL frequency table in i2c library folder
    initTWI(0,5);


    // this will write value 137 in meorylocation
    // 1000 of device 80
    EEWriteByte(80, 1000, 137);    
    LCDWriteStringXY(0,0, "Writing...");
    
    
    _delay_ms(1000);
    LCDClear();
    LCDWriteStringXY(0,0, "Reading...");
    _delay_ms(1000);
    LCDClear();

    // This function call will read memory location 1000 
    // of device 80 into "byte" variable
    
    byte = EEReadByte(80, 1000);
    LCDWriteIntXY(0,0, byte,3);
        
    
    while(1);
}
------------------------------------------------------------------------*/