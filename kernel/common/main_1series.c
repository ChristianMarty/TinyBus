/*
 * tinyloader.c
 *
 * Created: 21.03.2023 22:49:11
 * Author : Christian
 */ 
#include <main.h>
#include "device.h"
#include "bootloader_1series.h"

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));
void InterruptVectorTable(void)
{
	asm("rjmp main"); //RESET
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+0):);	//CRCSCAN_NMI
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+2):);	//BOD_VLM
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+4):);	//PORTA_PORT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+6):);	//PORTB_PORT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+8):); //PORTC_PORT
	asm("rjmp tickTimer_interruptHandler"); //RTC_CNT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+12):); //RTC_PIT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+14):); //TCA0_OVF, TCA0_LUNF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+16):); //TCA0_HUNF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+18):); //TCA0_CMP0, TCA0_LCMP0
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+20):); //TCA0_CMP1, TCA0_LCMP1
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+22):); //TCA0_CMP2, TCA0_LCMP2
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+24):); //TCB0_INT
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+26):); //TCD0_OVF
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+28):); //TCD0_TRIG
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+30):); //AC0_AC
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+32):); //ADC0_RESRDY
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+34):); //ADC0_WCOMP
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+36):); //TWI0_TWIS
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+38):); //TWI0_TWIM
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+40):); //SPI0_INT
	asm("rjmp USART0_RX_interruptHandler"); //USART0_RXC 
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+44):); //USART0_DRE
	asm("rjmp USART0_TX_interruptHandler"); //USART0_TXC
	asm("rjmp %0" ::  "i" (AppBaseByteAddress+48):); //NVMCTRL_EE
	
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	asm("rjmp com_transmit_data");
}


FUSES = {
	.OSCCFG = FREQSEL_20MHZ_gc,
	.SYSCFG0 = CRCSRC_NOCRC_gc | RSTPINCFG_UPDI_gc,
	.SYSCFG1 = SUT_64MS_gc,
	.APPEND = 0x00, // Application data section disabled
	.BOOTEND = 0x0A // Boot section size = 0x0A * 256 bytes = 0x0A00 bytes
};


int main(void)
{
	asm("CLR R1");//		'Clear Register 
	asm("OUT 0x3F,R1");//		Out to I/O location 
	// Init Stackpointer
	asm("SER R28");		//		Set Register 
	asm("OUT 0x3D,R28");//		Out to I/O location 
	asm("LDI R29,0x3F");//		Load immediate 
	asm("OUT 0x3E,R29");//		Out to I/O location 
	
	// do_copy_data
	asm("LDI R17,hi8(__data_end)");//		Load immediate 
	asm("LDI R26,lo8(__data_start)");//		Load immediate 
	asm("LDI R27,hi8(__data_start)");//		Load immediate 
	asm("LDI R30,lo8(__data_load_start)");//		Load immediate 
	asm("LDI R31,hi8(__data_load_start)");//		Load immediate 
	asm("RJMP .L__do_copy_data_start");//	Relative jump 
	
	asm (".L__do_copy_data_loop:");
	asm("LPM R0,Z+");//			Load program memory and post-increment 
	asm("ST X+,R0");//			Store indirect and post-increment 
	
	asm (".L__do_copy_data_start:");
	asm("CPI R26,lo8(__data_end)");//		Compare with immediate 
	asm("CPC R27,R17");//		Compare with carry 
	asm("BRNE .L__do_copy_data_loop");//		Branch if not equal 
	
	// do_clear_bss
	asm("LDI R18,hi8(__bss_end)");//		Load immediate 
	asm("LDI R26,lo8(__bss_start)");//		Load immediate 
	asm("LDI R27,hi8(__bss_start)");//		Load immediate 
	asm("RJMP .do_clear_bss_start");//	Relative jump 
	
	asm (".do_clear_bss_loop:");
	asm("ST X+,__zero_reg__");//			Store indirect and post-increment 
	
	asm (".do_clear_bss_start:");
	asm("CPI R26,lo8(__bss_end)");//		Compare with immediate 
	asm("CPC R27,R18");//		Compare with carry 
	asm("BRNE .do_clear_bss_loop");//		Branch if not equal 
	
	CPU_CCP = CCP_IOREG_gc;
	CPUINT.CTRLA = 0x40; //Interrupt vectors are placed at the start of the BOOT section of the Flash
	
	device_init();
	device_run();	// this function contains the main while(1) loop
}
