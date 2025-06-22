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
#define ADIM_TOP 0xFF

void pwm_init(void)
{
	// analog PWM
	TCCR1A = 0b11110010;
	TCCR1B = 0b00011010;
	TCCR1C = 0b11000000;
	ICR1 = ADIM_TOP;
	
	// PWM
	TCCR2A = 0b11110010;
	TCCR2B = 0b00011001;
	TCCR2C = 0b11000000;
	ICR2 = PWM_TOP;
	
	TOCPMSA0 = 0b01010000;
	TOCPMSA1 = 0b00001111;
	
	TOCPMCOE = 0b00111100;
}

void pwm_setWw(uint16_t pwm, uint8_t analog)
 {	
	// PIN  PA3 / TOCC2 / ADIM 1
	if(analog != ADIM_TOP){
		analog = (ADIM_TOP - analog);
		OCR1B = analog;
		
		TCCR1A |= 0b00110000;
		PORTA  &= 0b11110111;
	}else{
		TCCR1A &= 0b11001111;
		PORTA  |= 0b00001000;
	}
	
	// PIN  PA5 / TOCC4 / PWM 1
	if(pwm != PWM_TOP){
		pwm = (PWM_TOP - pwm);
		OCR2B = pwm;
		
		TCCR2A |= 0b00110000;
		PORTA  &= 0b11011111;
	}else{
		TCCR2A &= 0b11001111;
		PORTA  |= 0b00100000;
	}
}

void pwm_setCw(uint16_t pwm, uint8_t analog)
{
	// PIN  PA4 / TOCC3 / ADIM 2 
	if(analog != ADIM_TOP){
		analog = (ADIM_TOP - analog);
		OCR1A = analog;
			
		TCCR1A |= 0b11000000;
		PORTA  &= 0b11101111;
	}else{
		TCCR1A &= 0b00111111;
		PORTA  |= 0b00010000;
	}
	
	// PIN  PA6 / TOCC5 / PWM 2
	if(pwm != PWM_TOP){
		pwm = (PWM_TOP - pwm);
		OCR2A = pwm;
		
		TCCR2A |= 0b11000000;
		PORTA  &= 0b10111111;
	}else{
		TCCR2A &= 0b00111111;
		PORTA  |= 0b01000000;
	}
}
