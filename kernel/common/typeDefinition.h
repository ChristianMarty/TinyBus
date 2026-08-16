//**********************************************************************************************************************
// FileName : typeDefinition.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef TYPE_DEFINITION_H_
#define TYPE_DEFINITION_H_

typedef enum {
	DeviceState_unknown = 0,
	DeviceState_appCrcCheck, 
	DeviceState_appCrcError, 
	DeviceState_appStarting, 
	DeviceState_appRunning,
	DeviceState_appShutdown,
	DeviceState_appStopped
} deviceState_t; // must be max one nibble (4 bit)

typedef struct {
	uint8_t address;
	uint16_t appCrc;
	deviceState_t deviceState:4;
	uint8_t carrierDetected:1;
	uint8_t reserved0:3;
} shared_t;

typedef struct {
	uint8_t headerVersion:2;
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
} applicationHeader_t;

#endif /* TYPE_DEFINITION_H_ */
