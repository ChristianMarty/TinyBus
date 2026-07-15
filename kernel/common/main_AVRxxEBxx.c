/*
 * tinyloader.c
 *
 * Created: 21.03.2023 22:49:11
 * Author : Christian
 */ 
#include <main.h>
#include "device.h"
#include "bootloader_AVRxxEBxx.h"

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));
void InterruptVectorTable(void)
{
	asm("rjmp main"); //RESET
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+0):);	//NMI
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+4):);	//BOD_VLM
	asm("nop");
	asm("rjmp tickTimer_interruptHandler");	//RTC_CNT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+12):); //RTC_PIT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+16):); //CCL_CCL
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+20):); //PORTA_PORT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+24):); //WEX0
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+38):); //TCA0_OVF
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+32):); //TCE0_CMP0
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+36):); //TCE0_CMP1
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+40):); //TCE0_CMP2
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+44):); //TCE0_CMP3
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+48):); //TCB0_INT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+52):); //TCB1_INT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+56):); //TWI0_TWIC
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+60):); //TWI0_TWIH
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+64):); //SPI0_INT
	asm("nop");
	asm("rjmp USART0_RX_interruptHandler"); //USART0_RXC
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+72):); //USART0_DRE
	asm("nop");
	asm("rjmp USART0_TX_interruptHandler"); //USART0_TXC
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+80):); //PORTD_PORT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+84):); //TCF0_INT 
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+88):); //AC0_AC
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+92):); //ADC0_ERROR
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+96):); //ADC0_RESRDY
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+100):); //ADC0_SAMPRDY
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+104):); //AC1_AC
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+108):); //PORTC_PORT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+112):); //PORTF_PORT
	asm("nop");
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+114):); //NVMCTRL_NVMREADY
	asm("nop");
	
	asm("rjmp com_transmitData");
	asm("nop");
	asm("rjmp device_readEepromAppSection");
	asm("nop");
	asm("rjmp device_writeEepromAppSection");
	asm("nop");
}


FUSES = {
	.OSCCFG = OSCHFFRQ_20M_gc,
	.SYSCFG0 = CRCSRC_NOCRC_gc | UPDIPINCFG_UPDI_gc,
	.SYSCFG1 = SUT_64MS_gc,
	//.APPEND = 0x00, // Application data section disabled
	.BOOTSIZE = 0x0F // Boot section size = 0x0F * 256 bytes = 0x0F00 bytes
};


int main(void)
{
	asm("CLR R1");//		'Clear Register 
	asm("OUT 0x3F,R1");//		Out to I/O location 
	
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

	PortInitialization();
	
	MainPowerOn();
	CPU_CCP = CCP_IOREG_gc;
	CPUINT.CTRLA = 0x40; //Interrupt vectors are placed at the start of the BOOT section of the Flash
	
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL.OSCHFCTRLA = 0x01; // Enable Auto-tune
	
	reset_watchdog();
			
	device_init();
	device_run();	// this function contains the main while(1) loop
}
