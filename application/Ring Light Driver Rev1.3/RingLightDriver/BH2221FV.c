/*
 * BH2221FV.c
 *
 */ 
#include "BH2221FV.h"


#define  DataLow() (PORTA &= 0xFD)
#define  DataHigh() (PORTA |= 0x02)

#define  ClockLow()  (PORTA &= 0x07)
#define  ClockHigh() (PORTA |= 0x08)

#define  LdLow() (PORTA &= 0xFB)
#define  LdHigh() (PORTA |= 0x04)


void BH2221FV_sendAll( uint8_t *data)
{
	BH2221FV_send( 8,data[ 0]); //R1
	BH2221FV_send( 7,data[ 1]); //G1
	BH2221FV_send( 6,data[ 2]); //B1
	BH2221FV_send( 5,data[ 3]); //W1
	
	BH2221FV_send( 4,data[ 4]); //R2
	BH2221FV_send( 3,data[ 5]); //G2
	BH2221FV_send(10,data[ 6]); //B2
	BH2221FV_send( 9,data[ 7]); //W2
	
	BH2221FV_send( 1,data[ 8]); //R3
	BH2221FV_send( 2,data[ 9]); //G3
	BH2221FV_send(11,data[10]); //B3
	BH2221FV_send(12,data[11]);	//W3
}
	

void BH2221FV_send(uint8_t channel, uint8_t data)
{
	if(channel < 1) return;
	if(channel > 12) return;
	
	LdLow();
	ClockLow();
	DataLow();
	
	// send address
	for(uint8_t i = 0; i<4; i++)
	{
		ClockLow();
		
		if(channel & 0x01) DataHigh();
		else DataLow();
		channel = channel>>1;
		
		ClockHigh();
	}
	
	// send data
	for(uint8_t i = 0; i<8; i++)
	{
		ClockLow();
		
		if(data & 0x80) DataHigh();
		else DataLow();
		data = data<<1;
		
		ClockHigh();
	}
	
	LdHigh();
	LdLow();
}

