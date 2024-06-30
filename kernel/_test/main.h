#define TEST_RUN
#include <stdint.h>
#include <stdbool.h>

#define CONTROLLER_ID 0x0001
#define HARDWARE_ID   0x0002

#define RAM_READ
#define EEPROM_READ
#define EEPROM_WRITE

// KERNEL Rev 1.1
#define KERNEL_VERSION_MAJOR 0x01
#define KERNEL_VERSION_MINOR 0x01

// Device HW Rev 2.2
#define HARDWARE_VERSION_MAJOR 0x02
#define HARDWARE_VERSION_MINOR 0x02


/******************************************************************************
Application memory configuration
******************************************************************************/

#define AppBaseByteAddress 0x0C00
#define AppRamStart 0x060
#define AppEepromStart 0x010


/******************************************************************************
Port Initialization
******************************************************************************/

#define PortInitialization() PORTA = 0x80; DDRA = 0x80; DDRB = 0x00;


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

/******************************************************************************
Test Specific
******************************************************************************/
#define sei()

