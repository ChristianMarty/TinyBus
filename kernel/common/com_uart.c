//**********************************************************************************************************************
// FileName : com_uart.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <main.h>
#include "com_uart.h"
#include "../common/typeDefinition.h"
#include "../common/protocol.h"

#define COBS_DELIMITER 0x55 // IMPORTANT! This must be defined before cobs_u8.h is included
#include "utility/cobs_u8.h"
#include "utility/softCRC.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEST_RUN
extern shared_t shared __attribute__((section (".shared")));
#endif

typedef enum {UART_IDLE, UART_TX, UART_TX_COMPLETE, UART_RX, UART_RX_COMPLETE} uartState_t;
	
#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
	#define USART0_RX_ENABLE  USART0.CTRLB |= 0x80
	#define USART0_RX_DISABLE USART0.CTRLB &= 0x7F
#endif

#ifdef ATTINYx41
	#define USART0_RX_ENABLE UCSR0B |= 0x90
	#define USART0_RX_DISABLE UCSR0B &= 0x6F
#endif

#ifdef RxTxLedEnable
	volatile int8_t rxLedTimer5ms;
#endif

volatile uint8_t uartBuffer[UartBufferSize];
volatile uint8_t uartBufferPosition;

volatile uint8_t txSize;
volatile uint8_t rxByteCount;

volatile uartState_t uartState; 
volatile uint8_t com_error;
volatile uint8_t uart_timeout_counter;
volatile uint8_t uart_carrierSenseTimeoutCounter;


#ifdef TEST_RUN
#define USART0_RX_ENABLE
#define USART0_RX_DISABLE
#define RxPinState true
extern uint8_t rxRegister;
shared_t shared;
void com_setUartIdle(void)
{
	uartState = UART_IDLE;
}
#endif

void com_setBaudrate(com_baudRate_t baudRate)
{	
#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx) 
	switch(baudRate){
#ifdef Baudrate300
		case BAUD_300:
		USART0.BAUD = 44444;
		break;
#endif
#ifdef Baudrate600
		case BAUD_600:
		USART0.BAUD = 22222;
		break;
#endif
#ifdef BaudRate1200
		case BAUD_1200:
		USART0.BAUD = 11111;
		break;
#endif
#ifdef BaudRate2400
		case BAUD_2400:
		USART0.BAUD = 5555;
		break;
#endif

		default:
		case BAUD_4800:
		USART0.BAUD = 2778;
		break;

#ifdef BaudRate9600
		case BAUD_9600:
		USART0.BAUD = 1389;
		break;
#endif
#ifdef BaudRate14400
		case BAUD_14400:
		USART0.BAUD = 925;
		break;
#endif
#ifdef BaudRate19200
		case BAUD_19200:
		USART0.BAUD = 694;
		break;
#endif
#ifdef BaudRate28800
		case BAUD_28800:
		USART0.BAUD = 462;
		break;
#endif
#ifdef BaudRate38400
		case BAUD_38400:
		USART0.BAUD = 347;
		break;
#endif
#ifdef BaudRate57600
		case BAUD_57600:
		USART0.BAUD = 231;
		break;
#endif
#ifdef BaudRate76800
		case BAUD_76800:
		USART0.BAUD = 173;
		break;
#endif
#ifdef BaudRate115200
		case BAUD_115200:
		USART0.BAUD = 115;
		break;
#endif
	}
#endif

#if defined(ATTINYx41)
	switch(baudRate){
#ifdef BaudRate300
		case BAUD_300:
		UBRR0 = 3328;
		break;
#endif
#ifdef BaudRate600
		case BAUD_600:
		UBRR0 = 1664;
		break;
#endif
#ifdef BaudRate1200
		case BAUD_1200:
		UBRR0 = 832;
		break;
#endif
#ifdef BaudRate2400
		case BAUD_2400:
		UBRR0 = 416;
		break;
#endif
		default:
		case BAUD_4800:
		UBRR0 = 207;
		break;
		
#ifdef BaudRate9600
		case BAUD_9600:
		UBRR0 = 103;
		break;
#endif
#ifdef BaudRate14400
		case BAUD_14400:
		UBRR0 = 68;
		break;
#endif
#ifdef BaudRate19200
		case BAUD_19200:
		UBRR0 = 51;
		break;
#endif
#ifdef BaudRate28800
		case BAUD_28800:
		UBRR0 = 34;
		break;
#endif
#ifdef BaudRate38400
		case BAUD_38400:
		UBRR0 = 25;
		break;
#endif
#ifdef BaudRate57600
		case BAUD_57600:
		UBRR0 = 17;
		break;
#endif
#ifdef BaudRate76800
		case BAUD_76800:
		UBRR0 = 12;
		break;
#endif
#ifdef BaudRate115200
		case BAUD_115200:
		UBRR0 = 8;
		break;
#endif
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
	uartState = UART_IDLE; 
	uartBufferPosition = 0;

	txSize = 0;
	rxByteCount = 0;

  	com_error = 0;
	uart_timeout_counter = 0;
	uart_carrierSenseTimeoutCounter = 0;
	shared.carrierDetected = RxPinState;
	
#ifdef RxTxLedEnable
	rxLedTimer5ms = 0;
#endif
	
#ifdef AVRxxEBxx
	PORTMUX.USARTROUTEA = 0x04; // Use PC1, PC2 for UART
	USART0.CTRLB = 0xC0;
	PORTC.DIRSET = 0x02; // TX as output
	USART0.CTRLC = 0x03; // 8N1
	USART0.CTRLA = 0xC0; // USART Receive / Transmit Complete Interrupt Enable	
#endif

#ifdef TINYAVR_1SERIES
	PORTMUX.CTRLB |= 0x01; // Use PA1, PA2 for UART
	USART0.CTRLB = 0xC0;
	PORTA.DIRSET = 0x02; // TX as output
	USART0.CTRLC = 0x03; // 8N1
	USART0.CTRLA = 0xC0; // USART Receive / Transmit Complete Interrupt Enable
#endif

#ifdef ATTINYx41
	uint8_t rxByte = UDR0;
	UCSR0A = 0b00000000;
	UCSR0C = 0b00000110;
	UCSR0B = 0b11011000; // normal
	rxByte = UDR0;
	
	REMAP = 0x01;  // Pin mapping
#endif

}

//----------------------------------------------------------------------------------------------------------------------  
void com_5msTickHandler(void)
{		
	if(uart_timeout_counter > UartTimeout){
		uartState = UART_IDLE;	
		uart_timeout_counter = 0;
		uartBufferPosition = 0;
		com_error = 0;
	}
	
	if(uartState != UART_IDLE){
		uart_timeout_counter ++;
	}	
	
	if(uart_carrierSenseTimeoutCounter > UartCarrierSenseTimeout){
		shared.carrierDetected = false;
	}else{
		uart_carrierSenseTimeoutCounter++;
		shared.carrierDetected = true;
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
	if(RxPinState){
		uart_carrierSenseTimeoutCounter = 0;
	}
	
	if(uartState == UART_RX_COMPLETE)
	{
		if((!com_error)&&(uartBufferPosition > 4))
		{
            uint8_t rx_dataSize = cobs_decode((uint8_t*)&uartBuffer[0], (const uint8_t*)&uartBuffer[1], uartBufferPosition-1); // uartBuffer[0] -> byte 0 is cobs 0 and can be ignored
			
			uint16_t crc_16 = crc16((uint8_t*)&uartBuffer[0], rx_dataSize);
			if(crc_16 == 0){
				com_receiveData(uartBuffer[0], (uint8_t*)&uartBuffer[1], (rx_dataSize - 3)); // -3 because 2 bytes of crc and data_buffer[0] is passed separately 
			}
		}
		com_error = 0;
		uartState = UART_IDLE;	
	}
}

//----------------------------------------------------------------------------------------------------------------------  
void USART0_RX_interruptHandler(void)
{	
	uart_timeout_counter = 0; // Reset UART Timeout 
	uart_carrierSenseTimeoutCounter = 0; // Reset Carrier Sense Timeout 
	
	if(uartState == UART_IDLE){
		uartState = UART_RX;
		uartBufferPosition = 0;
	}
	
	if(uartState != UART_RX) return;
	
	
#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx) 
	if(USART0.RXDATAH & 0x46) {
		com_error = true; // check for frame error / data over-run
	}
	uint8_t rxByte =  USART0.RXDATAL;
#endif

#ifdef ATTINYx41
	if(UCSR0A & 0x14){
		com_error = true; // check for frame error / data over-run
	}
	uint8_t rxByte =  UDR0;
#endif

#ifdef TEST_RUN
	uint8_t rxByte = rxRegister;
#endif
	
	if(uartBufferPosition < UartBufferSize) {
        uartBuffer[uartBufferPosition] = rxByte;
    }else{
		com_error = true;
	}
	uartBufferPosition++;
	
	if((rxByte == COBS_DELIMITER)&&(uartBufferPosition > 1)){
		uartState = UART_RX_COMPLETE;
	}
}

//----------------------------------------------------------------------------------------------------------------------   
void transmitByte(void)
{
	if(txSize > uartBufferPosition) // transmitting
	{
		uint8_t tx_byte = uartBuffer[uartBufferPosition];
        uartBufferPosition ++;
		
	#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx) 
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
		uartState = UART_IDLE;
	#ifdef RxTxLedEnable
		TxLedOff();
	#endif
	}
}

//---------------------------------------------------------------------------------------------------------------------- 
void com_transmitData(uint8_t instructionByte, const uint8_t * data, uint8_t size, bool is_nAck)
{	
#ifdef RxTxLedEnable
	TxLedOn();
#endif

	uartState = UART_TX;
	USART0_RX_DISABLE;
	
	uint8_t i;
	uartBuffer[0] = COBS_DELIMITER;
	// Copy data to uart buffer
	uartBuffer[2] = instructionByte;
	for(i = 0; i<size; i++){
		uartBuffer[i+3] = data[i];
	}
	size++; // Because of instruction_byte
	
	if(is_nAck == 0){
		uint16_t crc = crc16((uint8_t*)&uartBuffer[2],size);
		
		uartBuffer[i+3] = (uint8_t) (crc >> 8); // CRC High
		uartBuffer[i+4] = (uint8_t) (crc & 0xFF); // CRC Low
	}else{
		uartBuffer[i+3] = 0; // CRC High
		uartBuffer[i+4] = 0; // CRC Low
	}
	size+=2; // Because of crc
	
	uint8_t tx_size = cobs_encode((uint8_t*)&uartBuffer[1], (const uint8_t*)&uartBuffer[2], size);
	tx_size++;
	
	// Start Transmitting
	txSize = tx_size;
	uartBufferPosition = 0;
	
	transmitByte();
}
//----------------------------------------------------------------------------------------------------------------------   
void USART0_TX_interruptHandler(void) 
{
	uart_timeout_counter = 0; // Reset UART Timeout
	transmitByte();

#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx) 
	USART0.STATUS = 0x40; // USART Transmit Complete Interrupt Clear
#endif
	
}
//----------------------------------------------------------------------------------------------------------------------   

#ifdef __cplusplus
}
#endif