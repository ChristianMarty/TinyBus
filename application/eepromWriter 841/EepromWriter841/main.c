//**********************************************************************************************************************
// FileName : main.c
// FilePath : /
// Author   : Christian Marty
// Date		: 01.02.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <stdbool.h>
#include <avr/io.h>

#include "sharedFunctions.h"
#include "typedef.h"

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 0

#define HARDWARE_ID 0x0000

#define APPLICATION_NAME "EEPROM Writer x41" // Max 18 characters

volatile shared_t shared __attribute__((section (".shared")));
volatile const application_header_t header __attribute__((section (".header"))) = {
	.autostart = true,
	.header_version = 0,
	.firmwareVersion_major = MAJOR_SW_REV,
	.firmwareVersion_minor = MINOR_SW_REV,
	.hardwareId_h = (uint8_t)(HARDWARE_ID>>8),
	.hardwareId_l = (uint8_t)(HARDWARE_ID),
	.name = APPLICATION_NAME
};

void app_main(void)
{
	if(shared.deviceState == APP_START)
	{	
		bootloader_updateEeprom(1,1);
		bootloader_updateEeprom(2,2);
	}

}

void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{

}

void app_5ms_tick(void)
{

}






