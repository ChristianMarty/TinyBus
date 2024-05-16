/*
 * bootloader.h
 *
 * Created: 03.08.2017 22:59:52
 * Author: Christian Marty
 */ 

#include "utility/softCRC.h"
#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/boot.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#define FlashByteSize (FLASHEND +1)

#define AppSize (FlashByteSize - AppBaseByteAddress) 
#define AppRamSize (RAMSIZE - AppRamAddress)
#define AppEepromSize (512 - AppEEPROMAddress)

#define AppBaseWordAddressH (AppBaseWordAddress>>8)
#define AppBaseWordAddressL (AppBaseWordAddress&0xFF)

#define FlashPageByteSize SPM_PAGESIZE

#define AppByteSize (FlashByteSize-AppBaseByteAddress)
#define AppBasePageAddress (AppBaseByteAddress>>6)

#define BootloadTransmitChunkSize 16 

#define CRC_INIT_VALUE 0xFFFF

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
	for(uint16_t i = AppBaseByteAddress; i< FlashByteSize; i += (FlashPageByteSize*4))
	{
		asm("WDR");
		boot_page_erase(i);
		boot_spm_busy_wait();      // Wait until the memory is erased.
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
	uint16_t CRC_value = CRC_INIT_VALUE;
	for(uint16_t i = AppBaseByteAddress; i < FlashByteSize-2; i++)
	{
		CRC_value = crc16_addByte(CRC_value,pgm_read_byte(i));
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
	crcValue = crc16_addByte(crcValue,pgm_read_byte(FlashByteSize-2));
	crcValue = crc16_addByte(crcValue,pgm_read_byte(FlashByteSize-1));

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
static inline void bootloader_writePage(uint16_t PageAddress, uint8_t *data)
{
	cli();
	
	for (uint8_t i=0; i<FlashPageByteSize; i++)
	{
		// Set up little-endian word.
		uint16_t word = data[i];
		i++;
		word |= (data[i] << 8);
		
		boot_page_fill ((PageAddress + i), word);
	}

	boot_page_write (PageAddress);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
	
	sei();
}

#endif /* BOOTLOADER_H_ */
