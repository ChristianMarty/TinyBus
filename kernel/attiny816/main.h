
#include <avr/io.h>
#include <stdbool.h>

#define TINYAVR_1SERIES

#define BROADCAST_ADDRESS  0x0F

#define KERNEL_REVISION 0x10 // KERNEL Rev 1.0
#define CONTROLLER_ID 0x01
#define DEVICE_ID 0x0003
#define DEVICE_HW_REV 0x20 // Device HW Rev 2.0

#define AppBaseByteAddress 0x0A00
#define AppRamAddress 0x0180
#define AppEEPROMAddress 0x0005

//#define APP_AUTOSTART