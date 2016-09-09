/********** Description **********
This library helps interfacing EEPROM such as 24c32 or 24c64
with Atmega 16/32/8
It uses I2C ports of atmega and I2C(TWI) protocol


PIN CONNECTION:
	For simplest application connect the following pincs
	VCC(PIN-8) to +5V
	GND(PIN-4) to 0V
	SDA(PIN-5) to PC1(PIN-23) of atmega
	SCL(PIN-6) to PC0(PIN-22) of atmega


NOTE:
	only one EEPROM can be interfaced in this simplest configuration.
	since A0:2 pins of EEPROM are not connected, the ROM is autometically 
	assigned a device address "0x00";

	WP(PIN-7) or Write Protect PIN if pulled up to VCC it blocks all write operation



FUNCTIONS (USER):
	1. void EEOpen();
	2. uint8_t EEWriteByte(uint16_t,uint8_t);
	3. uint8_t EEReadByte(uint16_t address);
 ---------------------------- */

#ifndef _24C64_H_
#define _24C64_H_

#define FALSE 0
#define TRUE 1

void EEOpen();
uint8_t EEWriteByte(uint16_t,uint8_t);
uint8_t EEReadByte(uint16_t address);

#endif





//------------------------------------------------------------------------------

#include <avr/io.h>
#include <util/delay.h>



void EEOpen()
{
	//Set up TWI Module
	TWBR = 5;
	TWSR &= (~((1<<TWPS1)|(1<<TWPS0)));

}

uint8_t EEWriteByte(uint16_t address,uint8_t data)
{
	do
	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));

		//Check status
		if((TWSR & 0xF8) != 0x08)
			return FALSE;

		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=0b10100000;	

		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
	
	}while((TWSR & 0xF8) != 0x18);
		

	//Now write ADDRH
	TWDR=(address>>8);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Now write ADDRL
	TWDR=(address);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Now write DATA
	TWDR=(data);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));

	//Wait untill Writing is complete
	_delay_ms(12);

	//Return TRUE
	return TRUE;

}

uint8_t EEReadByte(uint16_t address)
{
	uint8_t data;

	//Initiate a Dummy Write Sequence to start Random Read
	do
	{
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));

		//Check status
		if((TWSR & 0xF8) != 0x08)
			return FALSE;

		//Now write SLA+W
		//EEPROM @ 00h
		TWDR=0b10100000;	

		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);

		//Poll Till Done
		while(!(TWCR & (1<<TWINT)));
	
	}while((TWSR & 0xF8) != 0x18);
		

	//Now write ADDRH
	TWDR=(address>>8);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//Now write ADDRL
	TWDR=(address);

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x28)
		return FALSE;

	//*************************DUMMY WRITE SEQUENCE END **********************


	
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x10)
		return FALSE;

	//Now write SLA+R
	//EEPROM @ 00h
	TWDR=0b10100001;	

	//Initiate Transfer
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Poll Till Done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x40)
		return FALSE;

	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);

	//Wait till done
	while(!(TWCR & (1<<TWINT)));

	//Check status
	if((TWSR & 0xF8) != 0x58)
		return FALSE;

	//Read the data
	data=TWDR;

	//Put Stop Condition on bus
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	//Wait for STOP to finish
	while(TWCR & (1<<TWSTO));

	//Return TRUE
	return data;
}


		


/************** EXAMPLE CODE**************

#include <avr/io.h>
#include <util/delay.h>
#include "lib/lcd.h"
#include "lib/24c64.h"

int main()
{
   LCDInit(LS_NONE);
   _delay_ms(200);
   LCDClear();
   uint8_t data;
   //Init EEPROM
   EEOpen();
   EEWriteByte(10000,190);
   data=EEReadByte(10000);
   LCDWriteIntXY(0,0,data,3);
   while(1);

}

-----------------------------------------/