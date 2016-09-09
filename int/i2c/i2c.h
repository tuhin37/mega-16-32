/************************************** DESCRIPTION **************************************
This library is a work in progress
----------------------------------------------------------------------------------------*/




/*************************************** DEPENDENCY ***************************************/
#ifndef TW_START
	#include <util/twi.h>
#endif

#define I2C 1
/*----------------------------------------------------------------------------------------*/





/***************************************** MACROS *****************************************/

// set SCL Frequency
// reffer SCL Frequency table present in the same folder as this library
#define TWI_SET_SCL(twps, twbr) ({\
            TWBR = twbr;\
            TWSR = (twps & 3);\
        })


// put Stop condition on I2C bus
#define TWI_STOP() ({\
			TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);\
			while(TWCR & (1<<TWSTO));\
		})



//Poll Till Done
#define TWI_WAIT_TILL_DONE while(!(TWCR & (1<<TWINT)))



// gives status code masking prescaller bits from TWSR
#define TWI_STATUS (TWSR & 0xF8)


// reset the I2C interface
#define TWI_RESET() (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO)|(1<<TWEA))


//Initiate Transfer
#define TWI_TRANSFER() (TWCR=(1<<TWINT)|(1<<TWEN))


// Initiate Transffer with auto ACK
#define TWI_TRANSFER_ACK() (TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA))


// set own slave address
#define TWI_SLA_ADD(add, gc) ({\
            TWAR = (add<<1);\
            TWAR |= (1 & gc);\
        }) 

/*-----------------------------------------------------------------------------------------*/





/************************************** USER FUNCTIONS **************************************/
// set scl frequency
void initTWI(uint8_t twps, uint8_t twbr) {
	TWI_SET_SCL(twps, twbr);
}


// puts start condiiton 
// returns 0 if faild, else 1 if successfull
uint8_t TWI_START() {
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	//Poll Till Done
	TWI_WAIT_TILL_DONE;

	return TWI_STATUS;
}


//puts Repeat start condition 
// returns 0 if faild, else 1 if successfull
uint8_t TWI_RESTART() {
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	//Poll Till Done
	TWI_WAIT_TILL_DONE;

	
	return TWI_STATUS;
}


// puts an  SLA+W request on I2C bus
// returns 0 if faild, else 1 if successfull
uint8_t TWI_SLA_W(uint8_t address) {
	address = (address<<1); // append 8th bit (W bit => low bit) 
	

	//Now load SLA+W / SLA+R address (slave address(7 bit) with read/write bit appended at LSB position) 
	TWDR=address;	

	//Initiate Transfer
	TWI_TRANSFER();

	//Poll Till Done
	TWI_WAIT_TILL_DONE;

	// return status
	return TWI_STATUS;
	// possible status codes
	//	0x18 => TW_MT_SLA_ACK (Slave ACKed SLA+W)
	// 	0x20 => TW_MT_SLA_NACK (Slave Did not ACK SLA+W)
	// 	0x38 => TW_MT_ARB_LOST (Arbitration lost)
}


// puts an  SLA+R request on I2C bus
// returns 0 if faild, else 1 if successfull
uint8_t TWI_SLA_R(uint8_t address) {
	address = (address<<1) | 1;  // append 8th bit (R bit => high bit)
	do {
		
		//Now load SLA+W / SLA+R address (slave address(7 bit) with read/write bit appended at LSB position) 
		TWDR=address;	

		//Initiate Transfer
		TWI_TRANSFER();

		//Poll Till Done
		TWI_WAIT_TILL_DONE;

	}while(TWI_STATUS != TW_MR_SLA_ACK ); // 0x40 SLA written successfully. ACK from slave received
	return 1; // denotes successfull SLA write
}


// this can be data byte or memory address for EEPROM. (not device address on I2C bus tho)
// returns 0 if faild, else 1 if successfull
uint8_t TWI_ByteWrite(uint8_t byte) {
	//Now write ADDRH
	TWDR=(byte);

	//Initiate Transfer
	TWI_TRANSFER();

	//Poll Till Done
	TWI_WAIT_TILL_DONE;

	// return status
	return TWI_STATUS;
	// possible status codes
	// 0x28 => TW_MT_DATA_ACK (Slave ACKed byte receive)
	// 0x30 => TW_MT_DATA_NACK (Slave Did not ACK receive)
	// 0x38 => TW_MT_ARB_LOST (Arbitration lost)
}



// returns data.
uint8_t TWI_ByteRead() {
	
	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN);	

	
	//Wait till done
	TWI_WAIT_TILL_DONE;


	//Check status
	if((TWSR & 0xF8) != TW_MR_DATA_NACK) // 0x58 => nack received adter data transffer to master from slave
		return 0;

	// Return the data
	return TWDR;
}


// This function will read multiple (second argument) number of bytes 
// and will put them in *arr (first argument).
uint8_t TWI_multiByteRead(volatile uint8_t *arr, uint8_t number) {
	//Now enable Reception of data by clearing TWINT
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA); // ato ack enable

	uint8_t i;

	for(i=0; i<number; i++) {
		//Wait till done
		TWI_WAIT_TILL_DONE;
		if(i == number-1){  // only in last iteration
			//Check status
			if((TWSR & 0xF8) != TW_MR_DATA_NACK) {
				return 0;
			}
		}
		else {  
			//Check status
			if((TWSR & 0xF8) != TW_MR_DATA_ACK)
				return 0;
		}

		*(arr+i) = TWDR;
		if(i==number-2){ // seconds last iteration
			TWCR &= ~(1<<TWEA);
 		}
		if(i==number-1){
			TWI_STOP();
			break;
		}
		// ready for next packet
		TWCR |= (1<<TWINT); 
	}
	return 1;	// successful
}
/*-------------------------------------------------------------------------------------------*/
