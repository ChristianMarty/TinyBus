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

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

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

#define AppEEPROMAddress 0x0005

#define FlashByteSize PROGMEM_SIZE

#define AppSize (FlashByteSize - AppBaseByteAddress)
#define AppRamSize (RAMSIZE - AppRamAddress)
#define AppEepromSize (512 - AppEEPROMAddress)

#define AppBaseWordAddress (AppBaseByteAddress / 2)
#define AppBaseWordAddressH (AppBaseWordAddress>>8)
#define AppBaseWordAddressL (AppBaseWordAddress&0xFF)

#define FlashPageByteSize PROGMEM_PAGE_SIZE

#define AppByteSize (FlashByteSize-AppBaseByteAddress)
#define AppBasePageAddress (AppBaseByteAddress>>6)

#define BootloadTransmitChunkSize 16 

#define CRC_INIT_VALUE 0xFFFF

//**************************************************************************
//
//  Reboot
//
//	Parameter: None
//	Return value: None
//
//**************************************************************************  
static inline void bootloader_reboot(void)
{
	// Jump to start of bootloader
	asm("LDI R30, 0x00");
	asm("LDI R31, 0x00");
	asm("IJMP");
}

//**************************************************************************
//
//  Exit boot loader and start Application
//
//	Parameter: None
//	Return value: None
//
//**************************************************************************  
static inline void bootloader_startApp(void)
{
	// Jump to Application
	asm("LDI R30, %0 " : : "M" ( AppBaseWordAddressL):);	// Low Address Byte
	asm("LDI R31, %0 " : : "M"  (AppBaseWordAddressH):);	// High Address Byte
	asm("IJMP");
}

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
	
	uint16_t CRC_value = CRC_INIT_VALUE;
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
//	Parameter: None
//	Return value: Return 0 if App CRC is valid
//
//**************************************************************************
static inline uint16_t bootloader_checkAppCRC(void)
{
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	uint16_t CRC_value = bootloader_appCRC();

	for(uint16_t i = FlashByteSize-2; i< FlashByteSize; i++)
	{
		CRC_value = crc16_addByte(CRC_value, nvm_addr[i]);
	}
	
	return CRC_value;
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
	while(NVMCTRL.STATUS & 0x01);
	
	if(pageAddress % FlashPageByteSize == 0)
	{
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_PBC;
	}
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	for (uint8_t i=0; i<BootloadTransmitChunkSize; i++)
	{
		nvm_addr[pageAddress + i] = data[i];
	}
	
	if(pageAddress % FlashPageByteSize == 0x30)
	{
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_ERWP;
	}
	
	while(NVMCTRL.STATUS & 0x01);
	
	sei();
}

#endif /* BOOTLOADER_H_ */
