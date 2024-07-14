#include <avr/io.h>
#include <stdbool.h>

#define TINYAVR_1SERIES

#define CONTROLLER_ID 0x02
#define HARDWARE_ID 0x0003

#define RAM_READ
#define EEPROM_READ
#define EEPROM_WRITE


// Device HW Rev 2.0
#define HARDWARE_VERSION_MAJOR 0x02
#define HARDWARE_VERSION_MINOR 0x00


/******************************************************************************
Application memory configuration
******************************************************************************/

#define AppBaseByteAddress 0x0C00
#define AppRamStart 0x060
#define AppEepromStart 0x010


/******************************************************************************
Port Initialization
******************************************************************************/

#define PortInitialization();


/******************************************************************************
Configure pin for zero-power
******************************************************************************/

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
