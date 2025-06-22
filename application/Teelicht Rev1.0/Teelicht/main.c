/*
 * Dimmermodul.c
 *
 * Created: 08.05.2017 21:15:44
 * Author : Christian
 */ 

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#include "PWM.h"
#include "analog.h"
#include "SharedFunctions.h"
#include "typedef.h"

#include "led/dimmingCurve.h"
#include "led/perlin.h"
#include "random.h"

#define MAJOR_SW_REV 2
#define MINOR_SW_REV 4

#define MAJOR_HW_REV 2
#define MINOR_HW_REV 0

#define HARDWARE_ID 0x0003

#define APPLICATION_NAME "Teelicht" // Max 18 characters

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


bool perlinEnable = false;

void cmd_set_pwm(uint8_t pwm_ch, uint8_t *data);
void cmd_set_pwm_with_curve(uint8_t pwm_ch, uint8_t master, uint8_t value);

void perlin_enable(void);
void perlin_disable(void);
void perlin_handler(void);

void app_main(void)
{
	if(shared.deviceState == APP_START)
	{	
		pwm_set_ch1(0);
		pwm_set_ch2(0);
		pwm_set_ch3(0);
		pwm_set_ch4(0);
		pwm_init();
	
		PORTA &= 0x86;
		DDRA &= 0x86;
		DDRA |= 0x78;
	
		analog_init();
		
		uint16_t seed = analog_read_rand();
		seed = (seed<<8);
		seed |= analog_read_rand();
		random_initialize(seed);
		
		
		if(shared.carrierDetected == false){
			perlin_enable();
		}
	}
	
	perlin_handler();
	
	if(shared.deviceState == APP_SHUTDOWN)
	{
		// Turn LED off
		pwm_set_ch1(0);
		pwm_set_ch2(0);
		pwm_set_ch3(0);
		pwm_set_ch4(0);
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
		case 1:
		case 2:
		case 3:	cmd_set_pwm(instruction,data);
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
				 
		
		case 13: {
				perlin_enable();
				break;
		}
			
		// Voltage / Current
		case 14: {
				uint16_t v_read = analog_read_voltage();
				
				// Returns the Voltage reading in 1mV steps
				ack_data[0] = (uint8_t) ((v_read>>8) & 0x00FF);
				ack_data[1] = (uint8_t) (v_read & 0x00FF);
				ack_data[2] = 0;
				ack_data[3] = 0;
				ack_size = 4;
				break;
		}
						
		// Command not found
		default:
				cmd_error++;
				break;
	}
		
	if(!broadcast){
		com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
	}
}

void cmd_set_pwm(uint8_t pwm_ch, uint8_t *data)
{
	perlin_disable();
	
	uint16_t pwm_data = (data[1]<<8);
	pwm_data |= data[2];
	
	bool wfe = false;
	if(data[0]&0x80) wfe = true;
	
	pwm_fade(pwm_ch, (data[0]&0x7F), pwm_data,wfe);
}

void cmd_set_pwm_with_curve(uint8_t pwm_ch, uint8_t master, uint8_t value)
{
	perlin_disable();
	
	uint16_t output = dimmingCurve_getValue(value & 0x3F) * (master & 0x3F);
	pwm_fade(pwm_ch, 20, output, false);
}

volatile bool perlinTick = false;
void app_5ms_tick(void)
{
	pwm_5ms_tick();
	perlinTick = true;
}


// ---- PERLIN ----------------------------------------------------------------------------------------------------------------

#define PERLIN_SIZE 32
uint8_t seed[PERLIN_SIZE];
uint8_t output[PERLIN_SIZE];
uint16_t step = 0;

void perlin_enable(void)
{
	step = 0;
	perlinEnable = true;
}

void perlin_disable(void)
{
	if(!perlinEnable) return;
	
	perlinEnable = false;
	pwm_set_ch1(0x00);
	pwm_set_ch2(0x00);
	pwm_set_ch3(0x00);
	pwm_set_ch4(0X00);	
}

void perlin_handler(void)
{
	if(!perlinEnable) return;
	
	if(step < PERLIN_SIZE){
		seed[step] = random_getNumber()%PERLIN_SIZE;
	} 
	else if(step == PERLIN_SIZE)
	{	
		seed[0] = PERLIN_SIZE/2;
		perlin(&seed[0], &output[0], PERLIN_SIZE);
	}
	
	if(perlinTick){
		perlinTick = false;	
		
		pwm_set_ch4((output[step/4]*0x32) + 0x1FF);
		
		step++;
		if(step >= PERLIN_SIZE*4){
			step = 0;
		}
	}
}



