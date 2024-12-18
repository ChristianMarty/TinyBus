//**********************************************************************************************************************
// FileName : application.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 30.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <main.h>

#define AppHeaderByteOffset 0x20
#define AppInterruptVectorTableSize 30 // Number of Interrupt Vectors
#define AppBaseWordAddress (AppBaseByteAddress / 2)
#define AppEntrypoint (AppBaseWordAddress+(AppHeaderByteOffset/2))

// Shared Function Callbacks
#define APP_MAIN (AppEntrypoint)
#define APP_COM_RECEIVE_DATA_ADDR (AppEntrypoint+AppInterruptVectorTableSize+1)
#define APP_5MS_TICK_ADDR (AppEntrypoint+AppInterruptVectorTableSize+2)

typedef void (*PF_MAIN)(void);
static inline void app_main(void)
{
	((PF_MAIN) (APP_MAIN ))();
}

typedef void (*PF_VOID)(void);
static inline void app_5msTickHandler(void)
{
	((PF_VOID) (APP_5MS_TICK_ADDR ))();
}

typedef void (*PF_UINT82P1B)(uint8_t,uint8_t*,uint8_t, bool);
static inline void app_receiveDataHandler(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
	((PF_UINT82P1B) (APP_COM_RECEIVE_DATA_ADDR ))(instruction, data, size, broadcast);
}

#endif /* APPLICATION_H_ */