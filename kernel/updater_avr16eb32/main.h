//**********************************************************************************************************************
// FileName : main.h
// FilePath : /
// Author   : Christian Marty
// Date		: 15.11.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#define AVRxxEBxx

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 0

#define HARDWARE_ID 0x0000

#define APPLICATION_NAME "Kernel Updater" // Max 18 characters

/******************************************************************************
Application memory configuration
******************************************************************************/
#define AppBaseByteAddress 0x0F00
#define AppRamStart 0x060
#define AppEepromStart 0x010

/******************************************************************************
New Kernel Offset
******************************************************************************/
#define KernelAddressOffset 0x0250