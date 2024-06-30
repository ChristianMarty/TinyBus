//**********************************************************************************************************************
// FileName : device.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef DEVICE_H_
#define DEVICE_H_

#include <main.h>
#include "com_uart.h"
#include "tickTimer.h"

#include "../common/typedef.h"

#ifdef TINYAVR_1SERIES
    #define reset_watchdog() asm("WDR")
    extern shared_t shared __attribute__((section (".shared")));
#endif
#ifdef ATTINYx41
    #define reset_watchdog() asm("WDR")
    extern shared_t shared __attribute__((section (".shared")));
#endif
#ifdef TEST_RUN
    #define reset_watchdog()
    extern shared_t shared;
#endif

void device_init(void);
void device_run(void)  __attribute__ ((naked));

void device_eraseApp(void);
void device_reboot(void);

uint8_t device_updateAddress(uint8_t address);

#ifdef __cplusplus
}
#endif
#endif /* DEVICE_H_ */
