//**********************************************************************************************************************
// FileName : bootloader_x41.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "utility/softCRC.h"
#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/boot.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#define RamOffset 0x0100
#define RamSize RAMSIZE

#define EepromOffset 0x00810000
#define EepromSize E2SIZE

#define FlashByteSize (FLASHEND +1)
#define FlashPageByteSize SPM_PAGESIZE
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
	eeprom_busy_wait ();
	for(uint16_t i = AppBaseByteAddress; i < FlashByteSize; i += (FlashPageByteSize*4))
	{
		asm("WDR");
		boot_page_erase(i);
		boot_spm_busy_wait(); // Wait until the memory is erased.
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
	uint16_t crcValue = 0xFFFF;
	for(uint16_t i = AppBaseByteAddress; i < FlashByteSize-2; i++)
	{
		crcValue = crc16_addByte(crcValue, pgm_read_byte(i));
	}
	return crcValue;
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
	crcValue = crc16_addByte(crcValue, pgm_read_byte(FlashByteSize-2));
	crcValue = crc16_addByte(crcValue, pgm_read_byte(FlashByteSize-1));

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
	
	for (uint8_t i=0; i<FlashPageByteSize; i++)
	{
		// Set up little-endian word.
		uint16_t word = data[i];
		i++;
		word |= (data[i] << 8);
		
		boot_page_fill ((pageAddress + i), word);
	}

	boot_page_write(pageAddress);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
	
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

#endif /* BOOTLOADER_H_ */
