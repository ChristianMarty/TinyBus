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
	cli();
	while(NVMCTRL.STATUS&0x02);
	
	uint8_t *eemem_addr = (uint8_t *) address;
	sei();
	return eemem_addr[0];
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
	cli();
	while(NVMCTRL.STATUS&0x02);
	
	CPU_CCP = CCP_SPM_gc; // unlock
	NVMCTRL.CTRLA = NVMCTRL_PBC; // Page buffer clear

	uint8_t *eemem_addr = (uint8_t *) address;
	eemem_addr[0] = 0xFF;
	
	CPU_CCP = CCP_SPM_gc; // unlock
	NVMCTRL.CTRLA = NVMCTRL_ER; // erase
	while(NVMCTRL.STATUS&0x02);
	
	eemem_addr[0] = data;
	
	CPU_CCP = CCP_SPM_gc; // unlock
	NVMCTRL.CTRLA = NVMCTRL_WP; // Write page buffer to memory
		
	while(NVMCTRL.STATUS&0x02);
	sei();
}
