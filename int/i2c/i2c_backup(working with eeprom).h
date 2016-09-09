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



//Initiate Transfer
#define TWI_TRANSFER() (TWCR=(1<<TWINT)|(1<<TWEN))
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

	//Check status
	if(TWI_STATUS != TW_START) // 0x08 => START condition written successfully 
		return 0;
	return 1;
}


//puts Repeat start condition 
// returns 0 if faild, else 1 if successfull
uint8_t TWI_RESTART() {
	//Put Start Condition on TWI Bus
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	//Poll Till Done
	TWI_WAIT_TILL_DONE;

	//Check status
	if(TWI_STATUS != TW_REP_START) 
		return 0;
	return 1;
}



// puts an  SLA+W request on I2C bus
// returns 0 if faild, else 1 if successfull
uint8_t TWI_SLA_W(uint8_t address) {
	address = (address<<1); // append 8th bit (W bit => low bit) 
	do {

		//Now load SLA+W / SLA+R address (slave address(7 bit) with read/write bit appended at LSB position) 
		TWDR=address;	

		//Initiate Transfer
		TWI_TRANSFER();

		//Poll Till Done
		TWI_WAIT_TILL_DONE;

		if(TWI_STATUS == TW_REP_START)// if the SLA stated with REPEAT START then this will brake
			break;
		

	}while(TWI_STATUS != TW_MT_SLA_ACK); // 0x18 SLA written successfully. ACK from slave received
	return 1; // denotes successfull SLA write
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

	//Check status
	if(TWI_STATUS  != TW_MT_DATA_ACK)
		return 0;
	
	
	return 1; 
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
				outHigh(A7);
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











		





