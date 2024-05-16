
#include <avr/io.h>
#include <stdbool.h>

#define ATTINYx41

//#define APP_AUTOSTART

#define BROADCAST_ADDRESS  0x0F

#define KERNEL_REVISION 0x10 // KERNEL Rev 1.0
#define CONTROLLER_ID 0x01
#define DEVICE_ID 0x0002
#define DEVICE_HW_REV 0x22 // Device HW Rev 2.2

#define AppBaseByteAddress 0x0A00 
#define AppRamAddress 0x0180
#define AppEEPROMAddress 0x0005

#define  MainPowerOn()
#define  MainPowerOff()


/******************************************************************************
Enable and configure Rx and TX LED
******************************************************************************/
//#define RxTxLedEnable // Uncomment to Enable

#define RxTxLedOnTime 1 // in 5ms steps, int8_t -> max 127 -> 635ms

#define TxLedOn()
#define TxLedOff()

#define RxLedOn()
#define RxLedOff()