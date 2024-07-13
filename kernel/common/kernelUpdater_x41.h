//**********************************************************************************************************************
// FileName : kernelUpdater_x41.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 11.07.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <avr/boot.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef KERNEL_UPDATER_H_
#define KERNEL_UPDATER_H_

#define FlashByteSize (FLASHEND +1)
#define FlashPageByteSize SPM_PAGESIZE
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
	eeprom_busy_wait ();
	for(uint16_t i = 0; i < AppBaseByteAddress; i += (FlashPageByteSize*4))
	{
		asm("WDR");
		boot_page_erase(i);
		boot_spm_busy_wait(); // Wait until the memory is erased.
	}
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
	asm("WDR");
	boot_page_erase(AppBaseByteAddress);
	boot_spm_busy_wait(); // Wait until the memory is erased.
}

//**************************************************************************
//
//  Writes one Page of the Flash
//
//	Parameter: The page address, pointer to the data to write
//	Return value: None
//
//**************************************************************************
static inline void kernelUpdater_writePage(uint16_t pageAddress, uint8_t *data)
{
	asm("WDR");
	
	for (uint8_t i=0; i<FlashPageByteSize; i++)
	{
		// Set up little-endian word.
		uint16_t word = data[i];
		i++;
		word |= (data[i] << 8);
		
		boot_page_fill((pageAddress + i), word);
	}

	boot_page_write(pageAddress);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.
	
	asm("WDR");
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
