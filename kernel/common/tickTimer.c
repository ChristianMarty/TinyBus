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
#include "typeDefinition.h"

volatile uint16_t tickCounter;

#ifndef TEST_RUN
extern shared_t shared __attribute__((section (".shared")));
#endif

void tickTimer_init(void)
{
#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
	RTC.CLKSEL  = 0x00;
	RTC.INTCTRL = 0x01; // Overflow Interrupt Enable
	RTC.PER     = 0xA4;
	RTC.CTRLA   = 0x01; // Enable
#endif

#if defined(ATTINYx41)
	TCCR0A = 0;
	TCCR0B = 0b00000101; // Clock / 1024
	TIMSK0 = 0b00000001; // Overflow interrupt enable
	TCNT0 = 177;
#endif
	tickCounter = 0;
}

bool tickTimer_delay5ms(tickTimer_t *tickTimer, uint16_t delay)
{
    uint16_t t = tickCounter-tickTimer->counter;
	if(t < delay) {
        return false;
    }else{
		tickTimer->counter = tickCounter;
		return true;
	}
}

void tickTimer_reset(tickTimer_t *tickTimer)
{
	tickTimer->counter = tickCounter;
}

void tickTimer_interruptHandler(void)
{
#ifndef TEST_RUN
	com_5msTickHandler();
	if(shared.deviceState == DeviceState_appRunning){
		app_5msTickHandler();
	}
#endif
	tickCounter ++;

#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
	RTC.INTFLAGS |= 0x01;
#endif
}
