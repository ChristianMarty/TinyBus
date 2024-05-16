/*
 * PWM.c
 *
 * Created: 20.05.2017 11:49:04
 *  Author: Christian
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"

typedef struct  
{
	volatile uint16_t current_value;
	volatile uint16_t destination_value;
	volatile uint8_t fade_time_20ms;
	volatile bool wfe;
	volatile int16_t step_size;
}pwm_channel_t;

pwm_channel_t pwm_channel[4];


void pwm_init(void)
{
	TCCR1A = 0b11110010;
	TCCR1B = 0b00011001;
	TCCR1C = 0b11000000;
	ICR1 = 0xFFFF;
	
	TCCR2A = 0b11110010;
	TCCR2B = 0b00011001;
	TCCR2C = 0b11000000;
	ICR2 = 0xFFFF;
	
	TOCPMSA0 = 0b01010000;
	TOCPMSA1 = 0b00001111;
	
	TOCPMCOE = 0b00111100;
	
	for(uint8_t i = 0; i<4; i++)
	{
		pwm_channel[i].current_value = 0;
		pwm_channel[i].destination_value = 0;
		pwm_channel[i].fade_time_20ms = 0;
		pwm_channel[i].wfe = false;
		pwm_channel[i].step_size = 0;
	}
}

void pwm_5ms_tick(void)
{	
	for (uint8_t i = 0; i<4; i++)
	{
		if(pwm_channel[i].step_size != 0)
		{
			if(pwm_channel[i].current_value < pwm_channel[i].destination_value) // dimming up
			{
				// Prevent overflow
				if((0xFFFF - pwm_channel[i].current_value) > pwm_channel[i].step_size)
				{
					 pwm_channel[i].current_value += pwm_channel[i].step_size;
					 if(pwm_channel[i].current_value > pwm_channel[i].destination_value)
					 {
						 pwm_channel[i].current_value = pwm_channel[i].destination_value;
						 pwm_channel[i].step_size = 0;
					 }
				}
				else
				{
					 pwm_channel[i].current_value = pwm_channel[i].destination_value;
					 pwm_channel[i].step_size = 0;
				}
					 
				pwm_set(i, pwm_channel[i].current_value);
			}
			else if (pwm_channel[i].current_value > pwm_channel[i].destination_value)  // dimming down
			{
				// Prevent underflow
				if(pwm_channel[i].current_value > pwm_channel[i].step_size)
				{
					 pwm_channel[i].current_value -= pwm_channel[i].step_size;
					 if(pwm_channel[i].current_value < pwm_channel[i].destination_value)
					 {
						 pwm_channel[i].current_value = pwm_channel[i].destination_value;
						 pwm_channel[i].step_size = 0;
					 }
				}
				else 
				{
					pwm_channel[i].current_value = pwm_channel[i].destination_value;
					pwm_channel[i].step_size = 0;
				}
				
				pwm_set(i, pwm_channel[i].current_value);
			}
		}
		else
		{
			pwm_set(i, pwm_channel[i].current_value);
		}	
	}
}

int16_t calculate_fade_5ms_stepvalue(uint16_t pwm_value_old,uint16_t pwm_value_new, uint8_t fadetime_20ms)
{
	uint16_t pwm_difference = 0;
	
	if(pwm_value_new > pwm_value_old )
	{
		pwm_difference = pwm_value_new - pwm_value_old;
	}
	else if(pwm_value_new < pwm_value_old )
	{
		pwm_difference = pwm_value_old - pwm_value_new;
	}

	uint16_t fadetime_ticks = fadetime_20ms*4;	
	uint16_t fadeStep = (pwm_difference / fadetime_ticks);
	if(fadeStep <1) fadeStep = 1;
	
	return fadeStep;
}

void pwm_fade(uint8_t pwm_ch, uint8_t fadetime,uint16_t pwm_value, bool wfe)
{	
	pwm_channel[pwm_ch].destination_value = pwm_value;
	pwm_channel[pwm_ch].fade_time_20ms = fadetime;
	pwm_channel[pwm_ch].wfe = wfe;
	pwm_channel[pwm_ch].step_size = 0;
			
	if(wfe != true) pwm_execute(pwm_ch);	
}

void pwm_executeAll(void)
{
	for(int i = 0; i <4 ; i++)
	{
		pwm_execute(i);
	}
}

void pwm_execute(uint8_t pwm_ch)
{
	if(pwm_channel[pwm_ch].fade_time_20ms != 0)
	{
		pwm_channel[pwm_ch].step_size = calculate_fade_5ms_stepvalue(pwm_channel[pwm_ch].current_value, pwm_channel[pwm_ch].destination_value, pwm_channel[pwm_ch].fade_time_20ms);
	}
	else
	{
		pwm_channel[pwm_ch].step_size = 0;
		pwm_channel[pwm_ch].current_value = pwm_channel[pwm_ch].destination_value;
	}
}

void pwm_set(uint8_t pwm_ch,uint16_t pwm_value)
{
	switch(pwm_ch)
	{
		case 0:		pwm_set_ch1(pwm_value);
					break;
		case 1:		pwm_set_ch2(pwm_value);
					break;
		case 2:		pwm_set_ch3(pwm_value);
					break;
		case 3:		pwm_set_ch4(pwm_value);
					break;
	}
}


void pwm_set_ch1(uint16_t pwm_value)
{	
	pwm_channel[0].current_value = pwm_value;
	
	pwm_value = (0xFFFF - pwm_value);
	OCR1B = pwm_value;
}

void pwm_set_ch2(uint16_t pwm_value)
{
	pwm_channel[1].current_value = pwm_value;
	
	pwm_value = (0xFFFF - pwm_value);
	OCR1A = pwm_value;
}

void pwm_set_ch3(uint16_t pwm_value)
{
	pwm_channel[2].current_value = pwm_value;
	
	pwm_value = (0xFFFF - pwm_value);
	OCR2B = pwm_value;
}

void pwm_set_ch4(uint16_t pwm_value)
{
	pwm_channel[3].current_value = pwm_value;
	
	pwm_value = (0xFFFF - pwm_value);
	OCR2A = pwm_value;
}