//**********************************************************************************************************************
// FileName : com_uart.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <main.h>
#include "com_uart.h"
#include "../common/typedef.h"
#include "../common/protocol.h"

#define COBS_DELIMITER 0x55 // IMPORTANT! This must be defined before cobs_u8.h is included
#include "utility/cobs_u8.h"
#include "utility/softCRC.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {UART_IDLE, UART_TX, UART_TX_COMPLETE, UART_RX, UART_RX_COMPLETE} uart_state_t;

#ifdef TINYAVR_1SERIES
#define USART0_RX_ENABLE  USART0.CTRLB |= 0x80
#define USART0_RX_DISABLE USART0.CTRLB &= 0x7F
#endif

#ifdef ATTINYx41
#define USART0_RX_ENABLE UCSR0B |= 0x90
#define USART0_RX_DISABLE UCSR0B &= 0x6F
#endif

#ifdef TEST_RUN
#define USART0_RX_ENABLE
#define USART0_RX_DISABLE
extern uint8_t rxRegister;
#endif

#ifdef RxTxLedEnable
	volatile int8_t rxLedTimer5ms;
#endif

volatile uint8_t uart_buffer[UartBufferSize];
volatile uint8_t uart_buffer_position;

volatile uint8_t uart_tx_size;
volatile uint8_t rx_byte_count;

volatile uart_state_t uart_state; 
volatile uint8_t com_error;
volatile uint8_t uart_timeout_counter;

#ifdef TEST_RUN
    void com_setUartIdle(void)
    {
        uart_state = UART_IDLE;
    }
#endif

void com_setBaudrate(com_baudRate baudRate)
{	
#ifdef TINYAVR_1SERIES
	USART0.BAUD = baudsetting;
#endif

#ifdef ATTINYx41
	switch(baudRate){
		case BAUD_300:
		UBRR0 = 0x1A00;
		break;
		case BAUD_600:
		UBRR0 = 0x0D00;
		break;
		case BAUD_1200:
		UBRR0 = 0x0680;
		break;
		case BAUD_2400:
		UBRR0 = 0x0340;
		break;
		case BAUD_4800:
		UBRR0 = 0x01A0;
		break;
		case BAUD_9600:
		UBRR0 = 0x00CF;
		break;
		case BAUD_14400:
		UBRR0 = 0x008A;
		break;
		case BAUD_19200:
		UBRR0 = 0x0067;
		break;
	}
#endif
}

//**********************************************************************************************************************
//
//  Communication
//
//**********************************************************************************************************************
void com_init(void)
{
	uart_state = UART_IDLE; 
	uart_buffer_position = 0;

	uart_tx_size = 0;
	rx_byte_count = 0;

	com_error = 0;
	uart_timeout_counter = 0;
	
#ifdef RxTxLedEnable
	rxLedTimer5ms = 0;
#endif
	
#ifdef TINYAVR_1SERIES
	PORTMUX.CTRLB |= 0x01; // Use PA1, PA2 for UART
	USART0.BAUD = 925;
	USART0.CTRLB = 0xC0;
	PORTA.DIRSET = 0x02; // TX as output
	USART0.CTRLC = 0x03; // 8N1
	USART0.CTRLA = 0xC0; // USART Receive / Transmit Complete Interrupt Enable	
#endif

#ifdef ATTINYx41
	uint8_t rx_byte = UDR0;
	UCSR0A = 0b00000010 ; // Double the USART Transmission Speed
	UCSR0C = 0b00000110;
	UCSR0B = 0b11011000; // normal
	rx_byte = UDR0;
	
	REMAP = 0x01;  // Pin mapping
#endif
}

//----------------------------------------------------------------------------------------------------------------------  
void com_5msTickHandler(void)
{		
	if(uart_timeout_counter > UartTimeout)
	{
		uart_state = UART_IDLE;	
		uart_timeout_counter = 0;
		uart_buffer_position = 0;
		com_error = 0;
	}
	
	if(uart_state != UART_IDLE)
	{
		uart_timeout_counter ++;
	}	
	
#ifdef RxTxLedEnable
	if(rxLedTimer5ms == 0) RxLedOn();
	else if (rxLedTimer5ms == RxTxLedOnTime) RxLedOff();
	if(rxLedTimer5ms <= RxTxLedOnTime)  rxLedTimer5ms++;
#endif
}

//----------------------------------------------------------------------------------------------------------------------  
void com_handler(void)
{	
	if(uart_state != UART_RX_COMPLETE) return;
	
	if((!com_error)&&(uart_buffer_position > 4))
	{
		uint8_t rx_dataSize = 0;
		rx_dataSize = cobs_decode((uint8_t*)&uart_buffer[0], (const uint8_t*)&uart_buffer[1], uart_buffer_position-1); // uart_buffer[0] -> byte 0 is cobs 0 and can be ignored
			
		uint16_t crc_16 = crc16((uint8_t*)&uart_buffer[0], rx_dataSize);
		if(crc_16 == 0){
			com_receiveData(uart_buffer[0], (uint8_t*)&uart_buffer[1], (rx_dataSize - 3)); // -3 because 2 bytes of crc and data_buffer[0] is passed separately 
		}
	}
	com_error = 0;
	uart_state = UART_IDLE;	
}

//----------------------------------------------------------------------------------------------------------------------  
void USART0_RX_interruptHandler(void)
{	
	uart_timeout_counter = 0; // Reset UART Timeout 
	
	if(uart_state == UART_IDLE){
		uart_state = UART_RX;
		uart_buffer_position = 0;
	}
	
	if(uart_state != UART_RX) return;
	
#ifdef TINYAVR_1SERIES
	if(USART0.RXDATAH & 0x46) {
		com_error = true; // check for frame error / data over-run
	}
	uint8_t rx_byte =  USART0.RXDATAL;
#endif

#ifdef ATTINYx41
	if(UCSR0A & 0x14){
		com_error = true; // check for frame error / data over-run
	}
	uint8_t rx_byte =  UDR0;
#endif

#ifdef TEST_RUN
	uint8_t rx_byte = rxRegister;
#endif
	
	if(uart_buffer_position < UartBufferSize) {
        uart_buffer[uart_buffer_position] = rx_byte;
    }else{
		com_error = true;
	}
	uart_buffer_position++;
	
	if((rx_byte == COBS_DELIMITER)&&(uart_buffer_position > 1)){
		uart_state = UART_RX_COMPLETE;
	}
}

//----------------------------------------------------------------------------------------------------------------------   
void transmitByte(void)
{
	if(uart_tx_size > uart_buffer_position) // transmitting
	{
		uint8_t tx_byte = uart_buffer[uart_buffer_position];
        uart_buffer_position ++;
		
	#ifdef TINYAVR_1SERIES
		USART0.TXDATAL = tx_byte;
	#endif
	#ifdef ATTINYx41
		UDR0 = tx_byte;
    #endif
    #ifdef TEST_RUN
        com_txMockCallback(tx_byte);
    #endif

	}
	else // transmission completed
	{
		USART0_RX_ENABLE;
		uart_state = UART_IDLE;
	#ifdef RxTxLedEnable
		TxLedOff();
	#endif
	}
}
//---------------------------------------------------------------------------------------------------------------------- 
void com_transmitData(uint8_t instruction_byte,  uint8_t * data, uint8_t size, bool is_nAck)
{	
#ifdef RxTxLedEnable
	TxLedOn();
#endif

	uart_state = UART_TX;
	USART0_RX_DISABLE;
	
	uint8_t i = 0;
	uart_buffer[0] = COBS_DELIMITER;
	// Copy data to uart buffer
	uart_buffer[2] = instruction_byte;
	for(i = 0; i<size; i++){
		uart_buffer[i+3] = data[i];
	}
	size++; // Because of instruction_byte
	
	if(is_nAck == 0){
		uint16_t crc = crc16((uint8_t*)&uart_buffer[2],size);
		
		uart_buffer[i+3] = (uint8_t) (crc >> 8); // CRC High
		uart_buffer[i+4] = (uint8_t) (crc & 0xFF); // CRC Low
	}else{
		uart_buffer[i+3] = 0; // CRC High
		uart_buffer[i+4] = 0; // CRC Low
	}
	size+=2; // Because of crc
	
	uint8_t tx_size = cobs_encode((uint8_t*)&uart_buffer[1],(const uint8_t*)&uart_buffer[2],size);
	tx_size++;
	
	// Start Transmitting
	uart_tx_size = tx_size;
	uart_buffer_position = 0;
	
	transmitByte();
}
//----------------------------------------------------------------------------------------------------------------------   
void USART0_TX_interruptHandler(void) 
{
	uart_timeout_counter = 0; // Reset UART Timeout
	transmitByte();

#ifdef  TINYAVR_1SERIES
	USART0.STATUS = 0x40; // USART Transmit Complete Interrupt Clear
#endif
	
}
//----------------------------------------------------------------------------------------------------------------------   

#ifdef __cplusplus
}
#endif