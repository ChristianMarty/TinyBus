/*
 * bootloader.c
 *
 * Created: 31.07.2017 21:12:05
 * Author : Christian Marty
 */ 
#include <main.h>
#include "device.h"

#ifdef TINYAVR_1SERIES
	#include "../common/bootloader_1series.h"
#endif
#ifdef ATTINYx41
	#include "../common/bootloader_x41.h"
#endif

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));
void InterruptVectorTable(void)
{
	asm("rjmp main"); //RESET
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+0):);	//INT0
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+2):);	//PCINT0
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+4):);	//PCINT1
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+6):);	//WDT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+8):); //TIM1_CAPT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+10):); //TIM1_COMPA
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+12):); //TIM1_COMPB
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+14):); //TIM1_OVF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+16):); //TIM0_COMPA
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+18):); //TIM0_COMPB
	asm("rjmp tickTimer_interruptHandler");	//TIM0_OVF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+22):); //ANA_COMP0
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+24):); //ADC_READY
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+26):); //EE_RDY
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+28):); //ANA_COMP1
	asm("rjmp com_autobaudCapture_interruptHandler"); //TIM2_CAPT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+32):); //TIM2_COMPA
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+34):); //TIM2_COMPB
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+36):); //TIM2_OVF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+38):); //SPI
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+40):); //USART0_RXS
	asm("rjmp USART0_RX_interruptHandler");	//USART0_RXC 
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+44):); //USART0_DREasm("reti");//
	asm("rjmp USART0_TX_interruptHandler");	//USART0_TXC
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+48):); //USART1_RXS
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+50):); //USART1_RXC
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+52):); //USART1_DRE
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+54):); //USART1_TXC
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+56):); //TWI
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+58):); //RESERVED
	asm("nop");
	
	asm("rjmp com_transmit_data");
}

int main(void)
{	
	asm("CLR R1");		//		Clear Register R1
	asm("OUT 0x3F,R1");	//		Clear SREG 

// set stack pointer to 0x01FF (top of kernel SRAM)
	#define StackPointerInit  (RamSize+RamOffset-0x01)
	asm("LDI R29, %0" ::  "i" ((StackPointerInit>>8)&0xFF):);
	asm("LDI R28, %0" ::  "i" (StackPointerInit&0xFF):);
	asm("OUT 0x3E,R29");//		Out to I/O location 
	asm("OUT 0x3D,R28");//		Out to I/O location 
	
// clear RAM to all 0
	for(uint16_t i = RamOffset; i<RamOffset+RamSize; i++){
		*(uint8_t*)i = 0;
	}
	
	PORTA = 0x80;
	DDRA = 0x80;
	DDRB = 0x00;
	
	MainPowerOn();
	CCP = 0xD8;		// Configuration Change Protection
	WDTCSR = 0x47;	// Watchdog reset after 2s
	reset_watchdog();
			
	device_init();
	device_run();	// this function contains the main while(1) loop 
}
