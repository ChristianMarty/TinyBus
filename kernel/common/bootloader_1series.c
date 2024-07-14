//**********************************************************************************************************************
// FileName : bootloader_1series.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 14.07.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <avr/io.h>
#include "bootloader_1series.h"

//**************************************************************************
//
//  Read EEPROM
//
//	Parameter: Read address
//	Return value: EEPROM Data
//
//**************************************************************************
uint8_t bootloader_readEeprom(uint8_t *address)
{
	#warning "bootloader_readEeprom not implemented" // TODO: implement bootloader_readEeprom
	/*cli();
	while(EECR&0x02);
	EEARH = ((uint16_t)address)>>8;
	EEARL = ((uint16_t)address)&0xFF;
	EECR |= 0x01;
	
	uint8_t data = EEDR;
	sei();
	return data;*/
	
	return 0;
}

//**************************************************************************
//
//  Update EEPROM
//
//	Parameter: Write address, write data
//	Return value: None
//
//**************************************************************************
void bootloader_updateEeprom(uint8_t *address, uint8_t data)
{
	#warning "bootloader_updateEeprom not implemented" // TODO: implement bootloader_updateEeprom
	/*cli();
	while(EECR&0x02);
	
	// read data
	EEARH = ((uint16_t)address)>>8;
	EEARL = ((uint16_t)address)&0xFF;
	EECR |= 0x01;
	
	uint8_t oldData = EEDR;
	if(oldData != data){
		// write data
		EEDR = data;
		EECR |= 0x04;
		EECR |= 0x02;
		EECR = 0;
	}
	sei();*/
}

