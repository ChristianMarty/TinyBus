#include <avr/io.h>
#include <stdbool.h>

#define AVRxxEBxx

#define CONTROLLER_ID 0x03
#define HARDWARE_ID 0x0005

#define RAM_READ
#define EEPROM_READ
#define EEPROM_WRITE


// Device HW Rev 1.4
#define HARDWARE_VERSION_MAJOR 0x01
#define HARDWARE_VERSION_MINOR 0x04


/******************************************************************************
Application memory configuration
******************************************************************************/

#define AppBaseByteAddress 0x0F00
#define AppRamStart 0x060
#define AppEepromStart 0x010


/******************************************************************************
Port Initialization
******************************************************************************/

#define PortInitialization() PORTA.DIRSET = 0x7C; PORTF.DIRSET = 0x0C;
#define RxPinState (bool)(true)

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

#define TxLedOn() PORTA.OUTSET = 0x08
#define TxLedOff() PORTA.OUTCLR = 0x08

#define RxLedOn() PORTA.OUTSET = 0x04
#define RxLedOff() PORTA.OUTCLR = 0x04


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