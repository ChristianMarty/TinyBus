/*
 * PWM.c
 *
 * Created: 20.05.2017 11:49:04
 *  Author: Christian
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"

#define PWM_TOP 0xFFFF

typedef struct{
	volatile uint16_t current_value;
	volatile uint16_t destination_value;
	volatile uint8_t fade_time_20ms;
	volatile bool waitForExecution;
	volatile int16_t step_size;
}pwm_channel_t;

pwm_channel_t pwm_channel[4];

void pwm_init(void)
{
	TCCR1A = 0b11110010;
	TCCR1B = 0b00011001;
	TCCR1C = 0b11000000;
	ICR1 = PWM_TOP;
	
	TCCR2A = 0b11110010;
	TCCR2B = 0b00011001;
	TCCR2C = 0b11000000;
	ICR2 = PWM_TOP;
	
	TOCPMSA0 = 0b01010000;
	TOCPMSA1 = 0b00001111;
	
	TOCPMCOE = 0b00111100;
	
	for(uint8_t i = 0; i<4; i++){
		pwm_channel[i].current_value = 0;
		pwm_channel[i].destination_value = 0;
		pwm_channel[i].fade_time_20ms = 0;
		pwm_channel[i].waitForExecution = false;
		pwm_channel[i].step_size = 0;
	}
}


void pwm_updateFade(uint8_t index)
{
	pwm_channel_t *channel = &pwm_channel[index];
	
	if(channel->step_size != 0)
	{
		if(channel->current_value < channel->destination_value) // dimming up
		{
			// Prevent overflow
			if((PWM_TOP - channel->current_value) > channel->step_size)
			{
				channel->current_value += channel->step_size;
				if(channel->current_value > channel->destination_value)
				{
					channel->current_value = channel->destination_value;
					channel->step_size = 0;
				}
				}else{
				channel->current_value = channel->destination_value;
				channel->step_size = 0;
			}
			pwm_set(index, channel->current_value);
		}
		else if (channel->current_value > channel->destination_value)  // dimming down
		{
			// Prevent underflow
			if(channel->current_value > channel->step_size)
			{
				channel->current_value -= channel->step_size;
				if(channel->current_value < channel->destination_value)
				{
					channel->current_value = channel->destination_value;
					channel->step_size = 0;
				}
			}else{
				channel->current_value = channel->destination_value;
				channel->step_size = 0;
			}
			pwm_set(index, channel->current_value);
		}
	}else{
		
		pwm_set(index, channel->current_value);
	}
}

void pwm_5ms_tick(void)
{	
	for (uint8_t i = 0; i<4; i++){
		pwm_updateFade(i);	
	}
}

int16_t calculate_fade_5ms_stepvalue(uint16_t pwm_value_old, uint16_t pwm_value_new, uint8_t fadetime_20ms)
{
	uint16_t pwm_difference = 0;
	
	if(pwm_value_new > pwm_value_old){
		pwm_difference = pwm_value_new - pwm_value_old;
	}else if(pwm_value_new < pwm_value_old){
		pwm_difference = pwm_value_old - pwm_value_new;
	}

	uint16_t fadetime_ticks = fadetime_20ms*4;	
	uint16_t fadeStep = (pwm_difference / fadetime_ticks);
	if(fadeStep <1){
		fadeStep = 1;
	}
	
	return fadeStep;
}

void pwm_fade(uint8_t channel, uint8_t fadetime,uint16_t value, bool waitForExecution)
{	
	pwm_channel[channel].destination_value = value;
	pwm_channel[channel].fade_time_20ms = fadetime;
	pwm_channel[channel].waitForExecution = waitForExecution;
	pwm_channel[channel].step_size = 0;
			
	if(waitForExecution != true){
		pwm_execute(channel);
	}
}

void pwm_executeAll(void)
{
	for(int i = 0; i <4 ; i++){
		pwm_execute(i);
	}
}

void pwm_execute(uint8_t channel)
{
	if(pwm_channel[channel].fade_time_20ms != 0){
		pwm_channel[channel].step_size = calculate_fade_5ms_stepvalue(pwm_channel[channel].current_value, pwm_channel[channel].destination_value, pwm_channel[channel].fade_time_20ms);
	}else{
		pwm_channel[channel].step_size = 0;
		pwm_channel[channel].current_value = pwm_channel[channel].destination_value;
	}
}

void pwm_set(uint8_t channel, uint16_t value)
{
	switch(channel){
		case 0:		pwm_set_ch1(value);
					break;
		case 1:		pwm_set_ch2(value);
					break;
		case 2:		pwm_set_ch3(value);
					break;
		case 3:		pwm_set_ch4(value);
					break;
	}
}


void pwm_set_ch1(uint16_t value)
{	
	pwm_channel[0].current_value = value;
	
	// PIN  PA3 / TOCC2
	if(value != PWM_TOP){
		value = (PWM_TOP - value);
		OCR1B = value;
		
		TCCR1A |= 0b00110000;
		PORTA  &= 0b11110111;
	}else{
		TCCR1A &= 0b11001111;
		PORTA  |= 0b00001000;
	}
}

void pwm_set_ch2(uint16_t value)
{
	pwm_channel[1].current_value = value;
	
	// PIN  PA4 / TOCC3
	if(value != PWM_TOP){
		value = (PWM_TOP - value);
		OCR1A = value;
		
		TCCR1A |= 0b11000000;
		PORTA  &= 0b11101111;
	}else{
		TCCR1A &= 0b00111111;
		PORTA  |= 0b00010000;
	}
}

void pwm_set_ch3(uint16_t value)
{
	pwm_channel[2].current_value = value;
	
	// PIN  PA5 / TOCC4
	if(value != PWM_TOP){
		value = (PWM_TOP - value);
		OCR2B = value;
			
		TCCR2A |= 0b00110000;
		PORTA  &= 0b11011111;
	}else{
		TCCR2A &= 0b11001111;
		PORTA  |= 0b00100000;
	}
}

void pwm_set_ch4(uint16_t value)
{
	pwm_channel[3].current_value = value;
	
	// PIN  PA6 / TOCC5
	if(value != PWM_TOP){
		value = (PWM_TOP - value);
		OCR2A = value;
		
		TCCR2A |= 0b11000000;
		PORTA  &= 0b10111111;
	}else{
		TCCR2A &= 0b00111111;
		PORTA  |= 0b01000000;
	}
}