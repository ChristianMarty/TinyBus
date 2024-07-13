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
void app_5ms_tick(void);
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast);
*/
#endif /* SHAREDFUNCTIONS_H_ */