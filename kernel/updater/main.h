#include <avr/io.h>
#include <stdbool.h>

#define ATTINYx41

#define CONTROLLER_ID 0x01
#define HARDWARE_ID 0x0002

// KERNEL Rev 1.2
#define KERNEL_VERSION_MAJOR 0x01
#define KERNEL_VERSION_MINOR 0x02

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
New Kernel Offset
******************************************************************************/
#define KernelAddressOffset 0x0170