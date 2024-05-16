/*
 * Dimmermodul.c
 *
 * Created: 08.05.2017 21:15:44
 * Author : Christian
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "PWM.h"
#include "SharedFunctions.h"
#include "typedef.h"

void init_timer0(void);
void wait (uint16_t wait_time);
volatile uint16_t wait_takt;

void hv_enable(uint8_t enable);

#define MAJOR_SW_REV 2
#define MINOR_SW_REV 1

#define MAJOR_HW_REV 2
#define MINOR_HW_REV 0

volatile shared_t shared __attribute__((section (".shared")));

void app_main(void)
{
	if(shared.deviceState == APP_START)
	{
		pwm_init();
		pwm_update(0);
		hv_enable(0);
		PORTC.DIRSET = 0x04;
	}
	
	// Add main code here
	
	if(shared.deviceState == APP_SHUTDOWN)
	{
		pwm_update(0);
		hv_enable(0);
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
		case 0:		pwm_update(data[0]);
		break;
			
		case 1:		hv_enable(data[0]);
		break;
			
		// send some data
		case 2:
		ack_data[0] = 'A';
		ack_data[1] = 'B';
		ack_data[2] = 'C';
		ack_data[3] = 'D';
		ack_size = 4;
		break;
			
		// Command not found
		default:	cmd_error++;
		break;
	}
		
	if (!broadcast) com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
}

void app_5ms_tick(void)
{

}

void hv_enable(uint8_t enable)
{
	if(enable != 0) PORTC.OUTSET = 0x04;
	else PORTC.OUTCLR = 0x04;
}






