//**********************************************************************************************************************
// FileName : main.c
// FilePath : /
// Project  : Ring Light Driver
// Author   : Christian Marty
// Date		: 17.08.2026
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"
#include <avr/io.h>

#include "sharedFunctions.h"
#include "typeDefinition.h"

#include "pwm.h"

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

#define HighVoltage_enable() PORTC.OUTSET = 0x04
#define HighVoltage_disable() PORTC.OUTCLR = 0x04

void app_main(void)
{
	if(shared.deviceState == DeviceState_appStarting)
	{
		pwm_init();
		pwm_update(0);
		HighVoltage_disable();
		PORTC.DIRSET = 0x04;
	}
	
	// Add main code here
	
	if(shared.deviceState == DeviceState_appShutdown)
	{
		pwm_update(0);
		HighVoltage_disable();
	}
}

// Com RX Complete Call Back
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
	uint8_t cmd_error = 0;
		
	switch(instruction)
	{
		case 0:{
				pwm_update(data[0]);
		} break;
			
		case 1:{
				if(data[0])	HighVoltage_enable();
				else HighVoltage_disable();
		} break;
		
		default:{
			cmd_error++;
		} break;
	}
		
	if (!broadcast) com_transmit_data(instruction, 0, 0, cmd_error);
}

void app_5ms_tick(void)
{

}








