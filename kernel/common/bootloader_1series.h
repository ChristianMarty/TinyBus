//**********************************************************************************************************************
// FileName : bootloader_1series.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 14.07.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#include "utility/softCRC.h"
#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

enum{
	NVMCTRL_NC = 0,
	NVMCTRL_WP = 1,
	NVMCTRL_ER = 2,
	NVMCTRL_ERWP = 3,
	NVMCTRL_PBC = 4,
	NVMCTRL_CHER = 5,
	NVMCTRL_EEER = 6,
	NVMCTRL_WFU = 7
};

#define RamOffset RAMSTART
#define RamSize RAMSIZE

#define EepromOffset EEPROM_START
#define EepromSize EEPROM_SIZE

#define FlashByteSize PROGMEM_SIZE
#define FlashPageByteSize PROGMEM_PAGE_SIZE
#define AppFlashStart AppBaseByteAddress

#define AppBaseWordAddressH (AppBaseWordAddress>>8)
#define AppBaseWordAddressL (AppBaseWordAddress&0xFF)

#define BootloadTransmitChunkSize 16

//**************************************************************************
//
//  Erases the Application
//
//	Parameter: None
//	Return value: None
//
//**************************************************************************
static inline void bootloader_eraseAppSection(void)
{
	cli(); 

	CPU_CCP = 0x9D;
	NVMCTRL.CTRLA = NVMCTRL_PBC;
	
	while(NVMCTRL.STATUS & 0x01);
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	volatile uint16_t i = 0;
	for(i = AppBaseByteAddress; i< FlashByteSize; i += PROGMEM_PAGE_SIZE)
	{
		CPU_CCP = 0x9D;
		nvm_addr[i] = 0xFF;
		
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_ER;
		
		while(NVMCTRL.STATUS & 0x01);
	}
	sei();
}

//**************************************************************************
//
//  Calculates the CRC16
//
//	Parameter: None
//	Return value: The CRC16 checksum of the Application
//
//**************************************************************************
static inline uint16_t bootloader_appCRC(void)
{
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	uint16_t CRC_value = 0xFFFF;
	for(uint16_t i = AppBaseByteAddress; i<FlashByteSize-2; i++)
	{
		CRC_value = crc16_addByte(CRC_value, nvm_addr[i]);
	}
	return CRC_value;
}

//**************************************************************************
//
//  Checks the CRC16
//
//	Parameter: App CRC
//	Return value: Return 0 if App CRC is valid
//
//**************************************************************************
static inline uint16_t bootloader_checkAppCRC(uint16_t crcValue)
{
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
		
	crcValue = crc16_addByte(crcValue,nvm_addr[FlashByteSize-2]);
	crcValue = crc16_addByte(crcValue,nvm_addr[FlashByteSize-1]);

	return crcValue;
}

//**************************************************************************
//
//  Writes one Page of the Flash
//
//	Parameter: The page address, pointer to the data to write
//	Return value: None
//
//**************************************************************************
static inline void bootloader_writePage(uint16_t pageAddress, uint8_t *data)
{
	cli();
	while(NVMCTRL.STATUS & 0x03);
	
	if(pageAddress % FlashPageByteSize == 0)
	{
		CPU_CCP = CCP_SPM_gc; // unlock
		NVMCTRL.CTRLA = NVMCTRL_PBC; // Page buffer clear
	}
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	for (uint8_t i=0; i<BootloadTransmitChunkSize; i++)
	{
		nvm_addr[pageAddress + i] = data[i];
	}
	
	if(pageAddress % FlashPageByteSize == 0x30)
	{
		CPU_CCP = CCP_SPM_gc; // unlock
		NVMCTRL.CTRLA = NVMCTRL_WP; // Write page buffer to memory
	}
	
	while(NVMCTRL.STATUS & 0x01);
	sei();
}

//**************************************************************************
//
//  Read EEPROM
//
//	Parameter: Read address
//	Return value: EEPROM Data
//
//**************************************************************************
uint8_t bootloader_readEeprom(uint8_t *address);

//**************************************************************************
//
//  Update EEPROM
//
//	Parameter: Write address, write data
//	Return value: None
//
//**************************************************************************
void bootloader_updateEeprom(uint8_t *address, uint8_t data);

//**************************************************************************
//
//  Read byte from flash
//
//	Parameter: Read byte address
//	Return value: Flash Data
//
//**************************************************************************
static inline uint8_t bootloader_readByte(uint16_t address)
{
	return pgm_read_byte(address);
}

#endif /* BOOTLOADER_H_ */
