
#include <avr/io.h>
#include <stdbool.h>

#define ATTINYx41

#define BROADCAST_ADDRESS  0x0F

#define CONTROLLER_ID 0x01
#define HARDWARE_ID 0x0002

// KERNEL Rev 1.0
#define KERNEL_VERSION_MAJOR 0x01
#define KERNEL_VERSION_MINOR 0x00

// Device HW Rev 2.2
#define HARDWARE_VERSION_MAJOR 0x02 
#define HARDWARE_VERSION_MINOR 0x02 

#define AppBaseByteAddress 0x0A00

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