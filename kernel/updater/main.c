/*
 * Dimmermodul.c
 *
 * Created: 08.05.2017 21:15:44
 * Author : Christian
 */ 

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "SharedFunctions.h"
#include "typedef.h"
#include "kernelUpdater_x41.h"

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 0

#define MAJOR_HW_REV 1
#define MINOR_HW_REV 0

#define HARDWARE_ID 0x0000

#define APPLICATION_NAME "Kernel Updater" // Max 18 characters

volatile shared_t shared __attribute__((section (".shared")));
volatile const application_header_t header __attribute__((section (".header"))) = {
	.autostart = false,
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
		cli();
		kernelUpdater_eraseKernelSection();
		kernelUpdater_copy();
		kernelUpdater_eraseApplicationHeader();
		
		// Reboot
		// NOTO: this is not resetting the peripherals
		asm("LDI R30, 0x00");
		asm("LDI R31, 0x00");
		asm("IJMP");
	}
	
	// Add main code here
	
	if(shared.deviceState == APP_SHUTDOWN)
	{
	}
}

// Com RX Complete Call Back
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
		
}

void app_5ms_tick(void)
{
}






