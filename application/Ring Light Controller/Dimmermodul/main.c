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

#include "analog.h"
#include "SharedFunctions.h"
#include "typedef.h"
#include "BH2221FV.h"

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 0

#define MAJOR_HW_REV 1
#define MINOR_HW_REV 0

#define HARDWARE_ID 0x0003

#define APPLICATION_NAME "Ring Light" // Max 18 characters

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
		analog_init();
	}
	
	// Add main code here
	
	if(shared.deviceState == APP_SHUTDOWN)
	{

	}
}

// Com RX Complete Call Back
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
	uint8_t ack_data[4];
	uint8_t cmd_error = 0;
	uint8_t ack_size = 0;
		
	switch(instruction)
	{
		// Set single PWM channel
		case 0:
		BH2221FV_sendAll(data); break;
		
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7: 
		case 8: 
		case 9: 
		case 10: 
		case 11: 
		case 12: 
		BH2221FV_send(instruction,data[0]); break;

		
		// Voltage / Current
		case 14: {
				uint16_t v_read = analog_read_voltage();
				
				// Returns the Voltage reading in 1mV
				ack_data[0] = (uint8_t) (v_read>>8);
				ack_data[1] = (uint8_t) (v_read & 0x00FF);
				ack_size = 2;
				break;
		}
						
		// Command not found
		default:
				cmd_error++;
				break;
	}
		
	if(!broadcast) com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
}

void app_5ms_tick(void)
{

}






