//**********************************************************************************************************************
// FileName : main.c
// FilePath : /
// Author   : Christian Marty
// Date		: 15.11.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"
#include <avr/io.h>

#include "sharedFunctions.h"
#include "typeDefinition.h"

#include "kernelUpdater_AVRxxEBxx.h"

volatile shared_t shared __attribute__((section (".shared")));
volatile const applicationHeader_t header __attribute__((section (".header"))) = {
	.autostart = false,
	.headerVersion = 0,
	.firmwareVersion_major = MAJOR_SW_REV,
	.firmwareVersion_minor = MINOR_SW_REV,
	.hardwareId_h = (uint8_t)(HARDWARE_ID>>8),
	.hardwareId_l = (uint8_t)(HARDWARE_ID),
	.name = APPLICATION_NAME
};

void app_main(void)
{
	if(shared.deviceState == DeviceState_appStarting)
	{	
		cli();
		kernelUpdater_eraseKernelSection();
		kernelUpdater_copy();
		kernelUpdater_eraseApplicationHeader(); // destroy the app header so that the updater can only run once
		
		// Reboot
		// NOTO: this is not resetting the peripherals
		asm("LDI R30, 0x00");
		asm("LDI R31, 0x00");
		asm("IJMP");
	}
}

void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
}

void app_5ms_tick(void)
{
}
