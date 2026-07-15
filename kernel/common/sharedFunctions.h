//**********************************************************************************************************************
// FileName : sharedFunctions.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef SHAREDFUNCTIONS_H_
#define SHAREDFUNCTIONS_H_

#include <stdbool.h>
#include "main.h"

void InterruptVectorTable(void)  __attribute__ ((naked))  __attribute__ ((section (".vectors")));

#ifdef ATTINYx41

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

#define COM_TRANSMIT_DATA_ADDR 0x20
#define EEPROM_READ_DATA_ADDR 0x21
#define EEPROM_WRITE_DATA_ADDR 0x22

#endif

#ifdef AVRxxEBxx

void InterruptVectorTable(void)
{
	asm("rjmp app_main"); //Main
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//NMI
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//BOD_VLM
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//RTC_CNT
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//RTC_PIT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //CCL_CCL
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //PORTA_PORT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //WEX0
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCA0_OVF
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCE0_CMP0
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCE0_CMP1
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//TCE0_CMP2
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCE0_CMP3
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCB0_INT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TCB1_INT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TWI0_TWIC
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //TWI0_TWIH
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //SPI0_INT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //USART0_RXC
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //USART0_DRE
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //USART0_TXC
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //PORTD_PORT
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//TCF0_INT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //AC0_AC
	asm("nop");
	asm("rjmp dummy_interruptHandler");	//ADC0_ERROR
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //ADC0_RESRDY
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //ADC0_SAMPRDY
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //AC1_AC
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //PORTC_PORT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //PORTF_PORT
	asm("nop");
	asm("rjmp dummy_interruptHandler"); //NVMCTRL_NVMREADY

	asm("rjmp app_com_receive_data");
	asm("rjmp app_5ms_tick");
}

#define COM_TRANSMIT_DATA_ADDR 0x3E
#define EEPROM_READ_DATA_ADDR 0x40
#define EEPROM_WRITE_DATA_ADDR 0x42

#endif
void dummy_interruptHandler (void)
{
	while(1); // trip watchdog
}

void app_5ms_tick (void);
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast);

typedef void (*PF_UINT82PB)(uint8_t,uint8_t*,uint8_t,bool);
typedef bool (*PF_EEPROM)(uint16_t,uint8_t*,uint16_t);

void com_transmit_data(uint8_t instruction_byte, uint8_t *data, uint8_t size, bool is_nAck)
{
	((PF_UINT82PB) (COM_TRANSMIT_DATA_ADDR))(instruction_byte, data, size, is_nAck);
}

bool readEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size)
{
	return ((PF_EEPROM) (EEPROM_READ_DATA_ADDR))(offset, data, size);
}

bool writeEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size)
{
	return ((PF_EEPROM) (EEPROM_WRITE_DATA_ADDR))(offset, data, size);
}

#endif /* SHAREDFUNCTIONS_H_ */