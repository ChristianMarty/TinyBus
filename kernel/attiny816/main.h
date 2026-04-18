#include <avr/io.h>
#include <stdbool.h>

#define TINYAVR_1SERIES

#define CONTROLLER_ID 0x02
#define HARDWARE_ID 0x0001

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
#define RxPinState (bool)(true)

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
Supported baud rates -> Uncomment to Enable
******************************************************************************/

#define Baudrate300
#define Baudrate600
#define Baudrate1200
#define Baudrate2400
#define Baudrate4800
#define Baudrate9600
#define Baudrate14400
#define Baudrate19200
#define Baudrate28800
#define Baudrate38400
#define Baudrate57600
#define Baudrate76800
#define Baudrate115200