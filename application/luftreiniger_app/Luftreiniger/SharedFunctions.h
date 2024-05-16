/*
 * SharedFunctions.h
 *
 * Created: 01.08.2017 00:33:17
 *  Author: Christian
 */ 
#ifndef SHAREDFUNCTIONS_H_
#define SHAREDFUNCTIONS_H_

#include <stdbool.h>

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));
void InterruptVectorTable(void)
{
	asm("rjmp app_main"); //RESET
	asm("rjmp dummy_interruptHandler");	//CRCSCAN_NMI
	asm("rjmp dummy_interruptHandler");	//BOD_VLM
	asm("rjmp dummy_interruptHandler");	//PORTA_PORT
	asm("rjmp dummy_interruptHandler");	//PORTB_PORT
	asm("rjmp dummy_interruptHandler"); //PORTC_PORT
	asm("rjmp dummy_interruptHandler"); //RTC_CNT
	asm("rjmp dummy_interruptHandler"); //RTC_PIT
	asm("rjmp dummy_interruptHandler"); //TCA0_OVF, TCA0_LUNF
	asm("rjmp dummy_interruptHandler"); //TCA0_HUNF
	asm("rjmp dummy_interruptHandler"); //TCA0_CMP0, TCA0_LCMP0
	asm("rjmp dummy_interruptHandler"); //TCA0_CMP1, TCA0_LCMP1
	asm("rjmp dummy_interruptHandler"); //TCA0_CMP2, TCA0_LCMP2
	asm("rjmp dummy_interruptHandler"); //TCB0_INT
	asm("rjmp dummy_interruptHandler"); //TCD0_OVF
	asm("rjmp dummy_interruptHandler"); //TCD0_TRIG
	asm("rjmp dummy_interruptHandler"); //AC0_AC
	asm("rjmp dummy_interruptHandler"); //ADC0_RESRDY
	asm("rjmp dummy_interruptHandler"); //ADC0_WCOMP
	asm("rjmp dummy_interruptHandler"); //TWI0_TWIS
	asm("rjmp dummy_interruptHandler"); //TWI0_TWIM
	asm("rjmp dummy_interruptHandler"); //SPI0_INT
	asm("rjmp dummy_interruptHandler"); //USART0_RXC
	asm("rjmp dummy_interruptHandler"); //USART0_DRE
	asm("rjmp dummy_interruptHandler"); //USART0_TXC
	asm("rjmp dummy_interruptHandler"); //NVMCTRL_EE
	
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	asm("rjmp app_com_receive_data");
	asm("rjmp app_5ms_tick");
}

void dummy_interruptHandler (void)
{
	while(1); // trip watchdog
}

void app_5ms_tick (void);
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast);

/*

#define MAIN_ADDR 0x00

typedef void (*PF_MAIN)(bool,bool);

__inline__ void app_main (bool init, bool shutdown)
{
	((PF_MAIN) (MAIN_ADDR ))(init,shutdown);
}
*/

#define COM_TRANSMIT_DATA_ADDR 0x20
typedef void (*PF_UINT82PB)(uint8_t,uint8_t*,uint8_t,bool);
 void com_transmit_data(uint8_t instruction_byte, uint8_t *data, uint8_t size, bool is_nAck)
{
	((PF_UINT82PB) (COM_TRANSMIT_DATA_ADDR))(instruction_byte, data, size, is_nAck);
}

/*
void app_main(bool init);
void callback_5ms_tick(void);
void callback_com_receive_data(uint8_t instruction_byte, uint8_t *data, uint8_t size);
*/
#endif /* SHAREDFUNCTIONS_H_ */