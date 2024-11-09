//**********************************************************************************************************************
// FileName : device.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef DEVICE_H_
#define DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif
	
#include <main.h>
#include "com_uart.h"
#include "tickTimer.h"

#include "../common/typedef.h"
	
typedef struct{
	uint8_t deviceAddress;
	uint8_t hardwareVersionMajor;
	uint8_t hardwareVersionMinor;
	com_baudRate baudRate;
}settings_t;

#ifdef TINYAVR_1SERIES
    #define reset_watchdog() asm("WDR")
    extern shared_t shared __attribute__((section (".shared")));
	extern settings_t eeSettings  __attribute__((section(".eeprom")));
#endif
#ifdef ATTINYx41
    #define reset_watchdog() asm("WDR")
    extern shared_t shared __attribute__((section (".shared")));
	extern settings_t eeSettings  __attribute__((section(".eeprom")));
#endif
#ifdef TEST_RUN
    #define reset_watchdog()
    extern shared_t shared;
#endif

void device_init(void);
void device_run(void)  __attribute__ ((naked));

void device_eraseApp(void);
void device_reboot(void);

void device_saveBaudRate(void);
void device_setBaudRate(uint8_t baudRateIndex);

uint8_t device_updateAddress(uint8_t address);

//**************************************************************************
//  Read EEPROM App Section
//
//	Parameter: Offset within app section, read data, read size
//	Return value: true if successful
//	
//**************************************************************************
bool device_readEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size);

//**************************************************************************
//  Write EEPROM App Section
//
//	Parameter: Offset within app section, write data, write size
//	Return value: true if successful
//
//**************************************************************************
bool device_writeEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif /* DEVICE_H_ */
