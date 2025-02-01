#include <avr/io.h>
#include <stdbool.h>

#define ATTINYx41

#define CONTROLLER_ID 0x01
#define HARDWARE_ID 0x0003

#define RAM_READ
#define EEPROM_READ
#define EEPROM_WRITE


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

#define PortInitialization() PORTA = 0x80; DDRA = 0xEF; DDRB = 0x00;
#define RxPinState (bool)(PINB & 0x04)


/******************************************************************************
Configure pin for zero-power
******************************************************************************/

#define  MainPowerOn()
#define  MainPowerOff()


/****************************************************************************** 
Enable and configure Rx and TX LED 
******************************************************************************/

#define RxTxLedEnable // Uncomment to Enable 

#define RxTxLedOnTime 1 // in 5ms steps, int8_t -> max 127 -> 635ms

#define TxLedOn() (PORTA |= 0x02)
#define TxLedOff() (PORTA &= 0xFD)

#define RxLedOn() (PORTA |= 0x04)
#define RxLedOff() (PORTA &= 0xFB)


