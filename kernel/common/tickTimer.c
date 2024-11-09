//**********************************************************************************************************************
// FileName : tickTimer.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <main.h>

#ifndef TEST_RUN
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include "com_uart.h"
#endif

#include "tickTimer.h"
#include "typedef.h"

volatile uint16_t tickCounter;

#ifndef TEST_RUN
extern shared_t shared __attribute__((section (".shared")));
#endif

void tickTimer_init(void)
{
#ifdef TINYAVR_1SERIES
	RTC.CLKSEL = 0;
	RTC.INTCTRL |= 0x01; // Overflow Interrupt Enable
	RTC.PER = 0xAB;
	RTC.CTRLA |= 0x01;   // Enable
#endif

#ifdef ATTINYx41
	TCCR0A = 0;
	TCCR0B = 0b00000101;  // ckl/1024
	TIMSK0 = 0b00000001; // Overflow interrupt enable
	TCNT0 = 177;
#endif
	tickCounter = 0;
}

bool tickTimer_delay5ms(tickTimer_t *counter, uint16_t delay)
{
    uint16_t t = tickCounter-*counter;
	if(t < delay) {
        return false;
    }else{
		*counter = tickCounter;
		return true;
	}
}

void tickTimer_reset(tickTimer_t *counter)
{
	*counter = tickCounter;
}

void tickTimer_interruptHandler(void)
{
#ifndef TEST_RUN
	com_5msTickHandler();
	if(shared.deviceState == APP_RUNNING) app_5msTickHandler();
#endif
	tickCounter ++;

#ifdef TINYAVR_1SERIES
	RTC.INTFLAGS |= 0x01;
#endif
}
