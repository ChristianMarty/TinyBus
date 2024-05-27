//**********************************************************************************************************************
// FileName : typedef.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
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

typedef struct {
	uint8_t header_version:2;
	uint8_t reserved0:5;
	uint8_t autostart:1;
	
	uint8_t reserved1;
 
	uint8_t firmwareVersion_major;
	uint8_t firmwareVersion_minor;
	
	uint8_t hardwareId_h;
	uint8_t hardwareId_l;
	uint8_t hardwareVersion_major;
	uint8_t hardwareVersion_minor;
	
	uint8_t reserved2[6];
	
	uint8_t name[18];
} application_header_t;

#endif /* TYPEDEF_H_ */
