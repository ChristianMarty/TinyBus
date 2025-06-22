//**********************************************************************************************************************
// FileName : device.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifdef __cplusplus
extern "C" {
#endif
	
#include "device.h"
#include "typedef.h"
#include "main.h"


#ifdef TINYAVR_1SERIES
	#include "bootloader_1series.h"
	settings_t eeSettings  __attribute__((section(".eeprom"))) = {
		.deviceAddress = 0,
		.hardwareVersionMajor = HARDWARE_VERSION_MAJOR,
		.hardwareVersionMinor = HARDWARE_VERSION_MINOR,
		.baudRate = BAUD_4800
	};
#endif
#ifdef ATTINYx41
	#include "bootloader_x41.h"
	settings_t eeSettings  __attribute__((section(".eeprom"))) = {
		.deviceAddress = 0,
		.hardwareVersionMajor = HARDWARE_VERSION_MAJOR,
		.hardwareVersionMinor = HARDWARE_VERSION_MINOR,
		.baudRate = BAUD_4800
	};
#endif
#ifdef TEST_RUN
    #include "bootloader_test.h"
    uint8_t eeDeviceAddress = 0;
    void app_main()
    {}
#endif

shared_t shared __attribute__((section (".shared")));

uint8_t device_getAddress(void);
com_baudRate device_getBaudRate(void);
com_baudRate baudRate;

void device_init(void)
{
	shared.address = device_getAddress();
	if(shared.address >= 0xF){
		shared.address = 0x00; // in case the EEPROM was never programmed or address is out of range (>15)
	}
	
	device_setBaudRate(device_getBaudRate());
	
	shared.deviceState = APP_STOPPED;
	shared.appCrc = bootloader_appCRC();
	if(bootloader_checkAppCRC(shared.appCrc) != 0){
		shared.deviceState = APP_CRC_ERROR;
	}
	sei();
	tickTimer_init();
	com_setBaudrate(baudRate);
	com_init();
	
#ifdef TINYAVR_1SERIES
	bool watchdogReset = (RSTCTRL.RSTFR & 0x08);
#endif
#ifdef ATTINYx41
	bool watchdogReset = (MCUSR & 0x08);
#endif
#ifdef TEST_RUN
    bool watchdogReset = false;
#endif

// Autostart app
	if(shared.deviceState == APP_STOPPED && watchdogReset == false) //only in case watchdog reset was not triggered
	{
		uint8_t byte = bootloader_readByte(AppBaseByteAddress);
		if(byte&0x80){ // if autostart bit is set
			shared.deviceState = APP_START;
		}
	}
}

void device_run(void)
{
	while(1)
	{
		com_handler();
		reset_watchdog();
		
		switch(shared.deviceState)
		{
			case APP_CHECK_CRC:
				shared.appCrc = bootloader_appCRC();
				if(bootloader_checkAppCRC(shared.appCrc) == 0){
					shared.deviceState = APP_START;
				}else{
					shared.deviceState = APP_CRC_ERROR;
				}
				break;
				
			case APP_START:
				app_main();
				shared.deviceState = APP_RUNNING;
				break;
			
			case APP_RUNNING:
				app_main();
				break;
			
			case APP_SHUTDOWN:
				app_main();
				shared.deviceState = APP_STOPPED;
				break;
			
			case APP_CRC_ERROR:
			case APP_UNKNOWN:
			case APP_STOPPED:
				break;
		}
		
		reset_watchdog();
	}	
}

void device_eraseApp(void)
{
	if(shared.deviceState == APP_START || shared.deviceState == APP_RUNNING)
	{
		shared.deviceState = APP_SHUTDOWN;
		app_main();
	}
	
	shared.deviceState = APP_CRC_ERROR;
	bootloader_eraseAppSection();
}

void device_reboot(void)
{

#ifdef TINYAVR_1SERIES
	RSTCTRL.SWRR = 0x01;
#endif

#ifdef ATTINYx41
	// NOTO: this is not resetting the peripherals
	asm("LDI R30, 0x00");
	asm("LDI R31, 0x00");
	asm("IJMP");
#endif

}

uint8_t device_updateAddress(uint8_t address)
{
	// Note: Address changes apply AFTER restart of the device
	if((address > 0x00)&&(address < 0x0F)){
		
	#ifdef TINYAVR_1SERIES
		bootloader_updateEeprom((&eeSettings.deviceAddress)+EepromOffset, address);
	#endif
	#ifdef ATTINYx41
		bootloader_updateEeprom(&eeSettings.deviceAddress, address);
	#endif
		if(address == device_getAddress()){
			return true;
		}
	}
	return false;
}

uint8_t device_getAddress(void)
{
#ifdef TINYAVR_1SERIES
	return bootloader_readEeprom((&eeSettings.deviceAddress)+EepromOffset);
#endif
#ifdef ATTINYx41
	return bootloader_readEeprom(&eeSettings.deviceAddress);
#endif
}

com_baudRate device_getBaudRate(void)
{
#ifdef TINYAVR_1SERIES
	return bootloader_readEeprom((&eeSettings.baudRate)+EepromOffset);
#endif
#ifdef ATTINYx41
	return bootloader_readEeprom(&eeSettings.baudRate);
#endif
}

void device_setBaudRate(uint8_t baudRateIndex)
{
	if(baudRateIndex >= BAUD_LENGTH){
		baudRateIndex = BAUD_4800; // in case value is invalid
	}
	baudRate = baudRateIndex;
	com_setBaudrate(baudRate);
}

void device_saveBaudRate(void)
{
#ifdef TINYAVR_1SERIES
	return bootloader_updateEeprom((&eeSettings.baudRate)+EepromOffset, baudRate);
#endif
#ifdef ATTINYx41
	return bootloader_updateEeprom(&eeSettings.baudRate, baudRate);
#endif
}

bool device_readEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size)
{
	#define EepromAppSize (EepromSize-AppEepromStart)
	
	if(size+offset > EepromAppSize){
		return false;
	}
	
	#ifdef TINYAVR_1SERIES
	
		uint16_t baseAddress = ((uint16_t)&eeSettings.deviceAddress) + AppEepromStart + offset;
		for(uint16_t i = 0; i<size; i++){
			data[i] = bootloader_readEeprom((uint8_t*)(baseAddress+i));
		}
		return  true;
		
	#endif
	
	#ifdef ATTINYx41
	
		uint16_t baseAddress = ((uint16_t)&eeSettings.deviceAddress) + AppEepromStart + offset;
		for(uint16_t i = 0; i<size; i++){
			data[i] = bootloader_readEeprom((uint8_t*)(baseAddress+i));	
		}
		return  true;
		
	#endif
	
	return false;
}

bool device_writeEepromAppSection(uint16_t offset, uint8_t *data, uint16_t size)
{
	#define EepromAppSize (EepromSize-AppEepromStart)
	
	if(size+offset > EepromAppSize){
		return false;
	}
	
	#ifdef TINYAVR_1SERIES
		
		uint16_t baseAddress = ((uint16_t)&eeSettings.deviceAddress) + AppEepromStart + offset;
		for(uint16_t i = 0; i<size; i++){
			bootloader_updateEeprom((uint8_t*)(baseAddress+i), data[i]);
		}
		return  true;
		
	#endif
	
	#ifdef ATTINYx41
	
		uint16_t baseAddress = ((uint16_t)&eeSettings.deviceAddress) + AppEepromStart + offset;
		for(uint16_t i = 0; i<size; i++){
			bootloader_updateEeprom((uint8_t*)(baseAddress+i), data[i]);
		}
		return  true;
	
	#endif

	return false;
}

#ifdef __cplusplus
}
#endif