/*
 * PWM.c
 *
 * Created: 20.05.2017 11:49:04
 *  Author: Christian
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#include "PWM.h"

void pwm_init(void)
{
	/* enable write-protected register */
	CPU_CCP = CCP_IOREG_gc;
	TCD0.FAULTCTRL = TCD_CMPAEN_bm /* enable channel A */
	| TCD_CMPBEN_bm; /* enable channel B */
	
	PORTA.DIRSET = 0x30;
	
	TCD0.CTRLA  = 0x00; // Divide clock by 1
	
	//TCD0.CTRLC = 0x03;
	//TCD0.CTRLD = 0xAA;
	
	TCD0.CTRLA |= 0x01; // Enable
	
	pwm_update(0x7F);
}

void pwm_update(uint8_t value)
{
	if(value == 0) value = 1;
	if(value > 253) value = 253;
	
	uint16_t death_time = 0x01;
	uint16_t step = value;
	
	TCD0.CMPASET = death_time;
	TCD0.CMPACLR = step+death_time;
	
	TCD0.CMPBSET = step+death_time+1;
	TCD0.CMPBCLR = 0xFF + 0x22; // Counter top value
	
	TCD0.CTRLE |= 0x01; // Synchronize End of TCD Cycle Strobe
}