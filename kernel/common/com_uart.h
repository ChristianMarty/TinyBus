//**********************************************************************************************************************
// FileName : com_uart.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include <stdbool.h>

#ifndef COM_H_
#define COM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "utility/softCRC.h"
#include "utility/cobs_u8.h"
#ifndef TEST_RUN
#include "application.h"
#endif

#include "main.h"

#define UartBufferSize 30
#define UartTimeout 10 // In 5ms ticks -> 10 x 5ms = 50ms

void com_init(void);

void com_transmit_data(uint8_t instruction_byte, uint8_t *data, uint8_t size, bool is_nAck);
void com_receive_data(uint8_t instruction_byte, uint8_t *data, uint8_t size);

void com_handler(void);
void com_5ms_tick(void);

#ifndef TEST_RUN
void USART0_RX_interruptHandler(void)  __attribute__ ((signal));
void USART0_TX_interruptHandler(void)  __attribute__ ((signal));
void com_autobaudCapture_interruptHandler(void) __attribute__ ((signal));
#else
void USART0_RX_interruptHandler(void);
void USART0_TX_interruptHandler(void);
void com_autobaudCapture_interruptHandler(void);

void com_setUartIdle(void);
void com_txMockCallback(uint8_t byte);
#endif

#ifdef __cplusplus
}
#endif
#endif /* COM_H_ */