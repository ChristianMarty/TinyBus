#include <avr/io.h>
#include <stdbool.h>

#define ATTINYx41

#define CONTROLLER_ID 0x01
#define HARDWARE_ID 0x0005

#define RAM_READ
#define EEPROM_READ
#define EEPROM_WRITE

// KERNEL Rev 1.1
#define KERNEL_VERSION_MAJOR 0x01
#define KERNEL_VERSION_MINOR 0x01

// Device HW Rev 1.0
#define HARDWARE_VERSION_MAJOR 0x01
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

#define PortInitialization() PORTA = 0x80; DDRA = 0xFF; DDRB = 0x00;


/******************************************************************************
Configure pin for zero-power
******************************************************************************/

#define  MainPowerOn() (PORTA &= 0xFE)
#define  MainPowerOff() (PORTA |= 0x01)


/****************************************************************************** 
Enable and configure Rx and TX LED 
******************************************************************************/

#define RxTxLedEnable // Uncomment to Enable 

#define RxTxLedOnTime 1 // in 5ms steps, int8_t -> max 127 -> 635ms

#define TxLedOn() (PORTA |= 0x20)
#define TxLedOff() (PORTA &= 0xDF)

#define RxLedOn() (PORTA |= 0x40)
#define RxLedOff() (PORTA &= 0xBF)


