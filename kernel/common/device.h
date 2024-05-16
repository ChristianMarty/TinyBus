//**********************************************************************************************************************
// FileName : device.h
// FilePath : /
// Author   : Christian Marty
// Date		: 27.05.2023
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef DEVICE_H_
#define DEVICE_H_

#include <main.h>
#include <avr/eeprom.h>
#include "com_uart.h"
#include "tickTimer.h"

#include "../common/typedef.h"
extern shared_t shared __attribute__((section (".shared")));

#define reset_watchdog() asm("WDR")

void device_init(void);
void device_run(void);

void device_eraseApp(void);

void device_reboot(void);

uint8_t device_updateAddress(uint8_t address);


#endif /* DEVICE_H_ */
