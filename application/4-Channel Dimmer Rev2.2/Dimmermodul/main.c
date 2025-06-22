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

#include "PWM.h"
#include "analog.h"
#include "sharedFunctions.h"
#include "typedef.h"

#define MAJOR_SW_REV 2
#define MINOR_SW_REV 3

#define MAJOR_HW_REV 2
#define MINOR_HW_REV 0

#define HARDWARE_ID 0x0002

#define APPLICATION_NAME "4-Ch PWM Dimmer" // Max 18 characters

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

const PROGMEM uint16_t dimmingCurve[] = { 
0	,
1	,
2	,
3	,
4	,
5	,
6	,
7	,
8	,
9	,
10	,
11	,
12	,
13	,
14	,
16	,
18	,
22	,
25	,
30	,
34	,
39	,
44	,
50	,
57	,
64	,
72	,
81	,
90	,
100	,
111	,
123	,
135	,
148	,
161	,
176	,
192	,
208	,
225	,
243	,
263	,
283	,
304	,
326	,
349	,
374	,
399	,
426	,
453	,
482	,
512	,
544	,
576	,
610	,
645	,
682	,
720	,
759	,
800	,
842	,
885	,
930	,
977	,
1024
};

void cmd_set_pwm_with_curve(uint8_t pwm_ch, uint8_t master, uint8_t value)
{
	uint16_t output = pgm_read_word(&dimmingCurve[(value & 0x3F)]) * (master & 0x3F);
	pwm_fade(pwm_ch, 20, output, false);	
}

uint16_t dimmerValue[4];

void app_main(void)
{
	if(shared.deviceState == APP_START)
	{	
		pwm_set_ch1(0x00);
		pwm_set_ch2(0x00);
		pwm_set_ch3(0x00);
		pwm_set_ch4(0X00);
		pwm_init();
	
		PORTA &= 0x87;
		DDRA = 0xFC;
		DDRB = 0x00;
		PORTA |= 0x80;
	
		analog_init();
		
		if(shared.carrierDetected == false){
			readEepromAppSection(0, (uint8_t*)&dimmerValue[0], 8);
			
			pwm_set_ch1(dimmerValue[0]);
			pwm_set_ch2(dimmerValue[1]);
			pwm_set_ch3(dimmerValue[2]);
			pwm_set_ch4(dimmerValue[3]);
		}
	}
	
	// Add main code here
	
	if(shared.deviceState == APP_SHUTDOWN){
		// Turn LED off
		pwm_set_ch1(0x00);
		pwm_set_ch2(0x00);
		pwm_set_ch3(0x00);
		pwm_set_ch4(0X00);
	}
}

void cmd_set_pwm(uint8_t pwm_ch, uint8_t *data);
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
		case 1:
		case 2:
		case 3:	cmd_set_pwm(instruction, data);
				break;
			
		// Set channel 0 - 3
		case 4:	cmd_set_pwm(0, data);
				cmd_set_pwm(1, data+3);
				cmd_set_pwm(2, data+6);
				cmd_set_pwm(3, data+9);
				break;
			
		//Execute PWM operation
		case 5:	pwm_executeAll();
				break;
		
		// Set single PWM channel with curve
		case 8:
		case 9:
		case 10:
		case 11: cmd_set_pwm_with_curve(instruction-8, data[0], data[1]);
				 break;
		
		// Set Channel 0 - 3
		case 12: cmd_set_pwm_with_curve(0, data[0], data[1]);
				 cmd_set_pwm_with_curve(1, data[0], data[2]);
				 cmd_set_pwm_with_curve(2, data[0], data[3]);
				 cmd_set_pwm_with_curve(3, data[0], data[4]);
				 break;
			
		// Voltage / Current
		case 14: {
				uint16_t v_read = analog_read_voltage();
				uint16_t i_read = analog_read_current();
				
				// Returns the Voltage reading in 10mV and current reading in 1mA steps
				ack_data[0] = (uint8_t) (v_read>>8);
				ack_data[1] = (uint8_t) (v_read & 0x00FF);
				ack_data[2] = (uint8_t) (i_read>>8);
				ack_data[3] = (uint8_t) (i_read & 0x00FF);
				ack_size = 4;
				break;
		}
						
		// Command not found
		default:
				cmd_error++;
				break;
	}
		
	if(!broadcast) com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
}

void cmd_set_pwm(uint8_t pwm_ch, uint8_t *data)
{
	uint16_t pwm_data = (data[1]<<8);
	pwm_data |= data[2];
	
	bool waitForExecution = false;
	if(data[0]&0x80) waitForExecution = true;
	
	pwm_fade(pwm_ch,(data[0]&0x7F),pwm_data,waitForExecution);
}

void app_5ms_tick(void)
{
	pwm_5ms_tick();
}






