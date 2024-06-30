//**********************************************************************************************************************
// FileName : bootloader_x41.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "utility/softCRC.h"
#include "main.h"

#include <inttypes.h>

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_

#define RamOffset 0x0100
#define RamSize 0x0200

#define EepromOffset 0x00810000
#define EepromSize 0x0200

#define FlashByteSize (0x1FFF +1)
#define FlashPageByteSize 0x10
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
    return 0;
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
    return 0;
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
}

//**************************************************************************
//
//  Read EEPROM
//
//	Parameter: Read address
//	Return value: EEPROM Data
//
//**************************************************************************
static inline uint8_t bootloader_readEeprom(uint8_t *address)
{
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
static inline void bootloader_updateEeprom(uint8_t *address, uint8_t data)
{

}

//**************************************************************************
//
//  Read byte from flash
//
//	Parameter: The byte address
//	Return value: Value att address
//
//**************************************************************************
static inline uint8_t bootloader_readByte(uint16_t address)
{
    return 0;
}

#endif /* BOOTLOADER_H_ */
