//**********************************************************************************************************************
// FileName : typedef.h
// FilePath : /
// Author   : Christian Marty
// Date		: 27.05.2023
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef TYPEDEF_H_
#define TYPEDEF_H_

typedef enum {
	APP_UNKNOWN = 0,
	APP_CHECK_CRC, 
	APP_CRC_ERROR, 
	APP_START, 
	APP_RUNNING,
	APP_SHUTDOWN,
	APP_STOPPED
} device_state_t; // must be max one nibble (4 bit)

typedef struct {
	uint8_t address;
	uint16_t appCrc;
	device_state_t deviceState;
} shared_t;

#endif /* TYPEDEF_H_ */
