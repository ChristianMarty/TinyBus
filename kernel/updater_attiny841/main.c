//**********************************************************************************************************************
// FileName : main.c
// FilePath : /
// Author   : Christian Marty
// Date		: 15.11.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <avr/io.h>
#include <stdbool.h>
#include "typedef.h"
#include "kernelUpdater_x41.h"

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 0

#define HARDWARE_ID 0x0000

#define APPLICATION_NAME "Kernel Updater" // Max 18 characters

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

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));
void InterruptVectorTable(void)
{
	asm("rjmp app_main"); //Main
	asm("rjmp dummy_interruptHandler");	//INT0
	asm("rjmp dummy_interruptHandler");	//PCINT0
	asm("rjmp dummy_interruptHandler");	//PCINT1
	asm("rjmp dummy_interruptHandler");	//WDT
	asm("rjmp dummy_interruptHandler"); //TIM1_CAPT
	asm("rjmp dummy_interruptHandler"); //TIM1_COMPA
	asm("rjmp dummy_interruptHandler"); //TIM1_COMPB
	asm("rjmp dummy_interruptHandler"); //TIM1_OVF
	asm("rjmp dummy_interruptHandler"); //TIM0_COMPA
	asm("rjmp dummy_interruptHandler"); //TIM0_COMPB
	asm("rjmp dummy_interruptHandler");	//TIM0_OVF
	asm("rjmp dummy_interruptHandler"); //ANA_COMP0
	asm("rjmp dummy_interruptHandler"); //ADC_READY
	asm("rjmp dummy_interruptHandler"); //EE_RDY
	asm("rjmp dummy_interruptHandler"); //ANA_COMP1
	asm("rjmp dummy_interruptHandler"); //TIM2_CAPT
	asm("rjmp dummy_interruptHandler"); //TIM2_COMPA
	asm("rjmp dummy_interruptHandler"); //TIM2_COMPB
	asm("rjmp dummy_interruptHandler"); //TIM2_OVF
	asm("rjmp dummy_interruptHandler"); //SPI
	asm("rjmp dummy_interruptHandler"); //USART0_RXS
	asm("rjmp dummy_interruptHandler");	//USART0_RXC
	asm("rjmp dummy_interruptHandler"); //USART0_DRE
	asm("rjmp dummy_interruptHandler");	//USART0_TXC
	asm("rjmp dummy_interruptHandler"); //USART1_RXS
	asm("rjmp dummy_interruptHandler"); //USART1_RXC
	asm("rjmp dummy_interruptHandler"); //USART1_DRE
	asm("rjmp dummy_interruptHandler"); //USART1_TXC
	asm("rjmp dummy_interruptHandler"); //TWI
	asm("rjmp dummy_interruptHandler"); //RESERVED

	asm("rjmp app_com_receive_data");
	asm("rjmp app_5ms_tick");
}

void dummy_interruptHandler (void)
{
	while(1); // trip watchdog
}

void app_main(void)
{
	if(shared.deviceState == APP_START)
	{	
		cli();
		kernelUpdater_eraseKernelSection();
		kernelUpdater_copy();
		kernelUpdater_eraseApplicationHeader(); // destroy the app header so that the updater can only run once
		
		// Reboot
		// NOTO: this is not resetting the peripherals
		asm("LDI R30, 0x00");
		asm("LDI R31, 0x00");
		asm("IJMP");
	}
}

void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
}

void app_5ms_tick(void)
{
}
