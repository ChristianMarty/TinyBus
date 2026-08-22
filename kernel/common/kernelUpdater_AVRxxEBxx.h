//**********************************************************************************************************************
// FileName : kernelUpdater_AVRxxEBxx.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 22.08.2026
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#ifndef KERNEL_UPDATER_H_
#define KERNEL_UPDATER_H_

enum{
	NVMCTRL_NOCMD = 0x00,
	NVMCTRL_NOOP = 0x01,
	NVMCTRL_FLPW = 0x04,
	NVMCTRL_FLPERW = 0x05,
	NVMCTRL_FLPER = 0x08,
	NVMCTRL_FLMPER2 = 0x09,
	NVMCTRL_FLMPER4 = 0x0A,
	NVMCTRL_FLMPER8 = 0x0B,
	NVMCTRL_FLMPER16 = 0x0C,
	NVMCTRL_FLMPER32 = 0x0D,
	NVMCTRL_FLPBCLR = 0x0F,
	NVMCTRL_EEPW = 0x14,
	NVMCTRL_EEPERW = 0x15,
	NVMCTRL_EEPER = 0x17,
	NVMCTRL_EEPBCLR = 0x1F,
	NVMCTRL_CHER = 0x20,
	NVMCTRL_EECHER = 0x30,
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
//  Erases the old Kernel
//
//	Parameter: None
//	Return value: None
//
//**************************************************************************
static inline void kernelUpdater_eraseKernelSection(void)
{
	cli();

	CPU_CCP = 0x9D;
	NVMCTRL.CTRLA = NVMCTRL_FLPBCLR;
	
	while(NVMCTRL.STATUS & 0x01);
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	volatile uint16_t i = 0;
	for(i = 0; i< AppBaseByteAddress; i += PROGMEM_PAGE_SIZE)
	{
		CPU_CCP = 0x9D;
		nvm_addr[i] = 0xFF;
		
		CPU_CCP = CCP_SPM_gc;
		NVMCTRL.CTRLA = NVMCTRL_FLPER;
		
		while(NVMCTRL.STATUS & 0x01);
	}
	sei();
}

//**************************************************************************
//
//  Erases application header
//
//	Parameter: None
//	Return value: None
//
//**************************************************************************
static inline void kernelUpdater_eraseApplicationHeader(void)
{
	cli();

	CPU_CCP = 0x9D;
	NVMCTRL.CTRLA = NVMCTRL_FLPBCLR;
	
	while(NVMCTRL.STATUS & 0x01);
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	CPU_CCP = 0x9D;
	nvm_addr[AppBaseByteAddress] = 0xFF;

	CPU_CCP = CCP_SPM_gc;
	NVMCTRL.CTRLA = NVMCTRL_FLPER;

	while(NVMCTRL.STATUS & 0x01);
	
	sei();
}

//**************************************************************************
//
//  Writes one Page of the Flash
//
//	Parameter: The page address, pointer to the data to write
//	Return value: None
//
//**************************************************************************
static inline void kernelUpdater_writePage(uint16_t pageAddress, const uint8_t *data)
{
	cli();
	while(NVMCTRL.STATUS & 0x03);
	
	if(pageAddress % FlashPageByteSize == 0)
	{
		CPU_CCP = CCP_SPM_gc; // unlock
		NVMCTRL.CTRLA = NVMCTRL_FLPBCLR; // Page buffer clear
	}
	
	uint8_t *nvm_addr = (uint8_t *)MAPPED_PROGMEM_START + PROGMEM_START;
	
	for (uint8_t i=0; i<BootloadTransmitChunkSize; i++)
	{
		nvm_addr[pageAddress + i] = data[i];
	}
	
	if(pageAddress % FlashPageByteSize == 0x30)
	{
		CPU_CCP = CCP_SPM_gc; // unlock
		NVMCTRL.CTRLA = NVMCTRL_FLPW; // Write page buffer to memory
	}
	
	while(NVMCTRL.STATUS & 0x01);
	sei();
}

//**************************************************************************
//
//  Writes one Page of the Flash
//
//	Parameter: The page address, pointer to the data to write
//	Return value: None
//
//**************************************************************************
static inline void kernelUpdater_copy()
{
	uint16_t newKernelStartAddress = AppBaseByteAddress+KernelAddressOffset;
	
	for(uint16_t i = 0; i<AppBaseByteAddress; i+=FlashPageByteSize)
	{	
		uint8_t temp[FlashPageByteSize];
		for (uint8_t j = 0; j<FlashPageByteSize; j++)
		{
			temp[j] = pgm_read_byte(newKernelStartAddress+i+j);
		}
		
		kernelUpdater_writePage(i, &temp[0]);
	}
}

#endif /* KERNEL_UPDATER_H_ */
