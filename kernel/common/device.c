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
#include "typeDefinition.h"
#include "main.h"

#ifdef AVRxxEBxx
	#include "bootloader_AVRxxEBxx.h"
	settings_t eeSettings  __attribute__((section(".eeprom"))) = {
		.deviceAddress = 0,
		.hardwareVersionMajor = HARDWARE_VERSION_MAJOR,
		.hardwareVersionMinor = HARDWARE_VERSION_MINOR,
		.baudRate = BAUD_4800
	};
	shared_t shared __attribute__((section (".shared")));
#endif

#ifdef TINYAVR_1SERIES
	#include "bootloader_1series.h"
	settings_t eeSettings  __attribute__((section(".eeprom"))) = {
		.deviceAddress = 0,
		.hardwareVersionMajor = HARDWARE_VERSION_MAJOR,
		.hardwareVersionMinor = HARDWARE_VERSION_MINOR,
		.baudRate = BAUD_4800
	};
	shared_t shared __attribute__((section (".shared")));
#endif

#ifdef ATTINYx41
	#include "bootloader_x41.h"
	settings_t eeSettings  __attribute__((section(".eeprom"))) = {
		.deviceAddress = 0,
		.hardwareVersionMajor = HARDWARE_VERSION_MAJOR,
		.hardwareVersionMinor = HARDWARE_VERSION_MINOR,
		.baudRate = BAUD_4800
	};
	shared_t shared __attribute__((section (".shared")));
#endif

#ifdef TEST_RUN
    #include "bootloader_test.h"
    uint8_t eeDeviceAddress = 0;
    void app_main()
    {}
#endif

uint8_t device_getAddress(void);
com_baudRate_t device_getBaudRate(void);
com_baudRate_t baudRate;

void device_init(void)
{
	shared.address = device_getAddress();
	if(shared.address >= 0xF){
		shared.address = 0x00; // in case the EEPROM was never programmed or address is out of range (>15)
	}
	
	device_setBaudRate(device_getBaudRate());
	
	shared.deviceState = DeviceState_appStopped;
	shared.appCrc = bootloader_appCRC();
	if(bootloader_checkAppCRC(shared.appCrc) != 0){
		shared.deviceState = DeviceState_appCrcError;
	}
	sei();
	tickTimer_init();
	com_setBaudrate(baudRate);
	com_init();
	
#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
	bool watchdogReset = (RSTCTRL.RSTFR & 0x08);
#endif
#ifdef ATTINYx41
	bool watchdogReset = (MCUSR & 0x08);
#endif
#ifdef TEST_RUN
    bool watchdogReset = false;
#endif

// Autostart app
	if(shared.deviceState == DeviceState_appStopped && watchdogReset == false) //only in case watchdog reset was not triggered
	{
		uint8_t byte = bootloader_readByte(AppBaseByteAddress);
		if(byte&0x80){ // if autostart bit is set
			shared.deviceState = DeviceState_appStarting;
		}
	}
}

void device_run(void)
{
	while(1)
	{
		com_handler();
		watchdogReset();
		
		switch(shared.deviceState)
		{
			case DeviceState_appCrcCheck:
				shared.appCrc = bootloader_appCRC();
				if(bootloader_checkAppCRC(shared.appCrc) == 0){
					shared.deviceState = DeviceState_appStarting;
				}else{
					shared.deviceState = DeviceState_appCrcError;
				}
				break;
				
			case DeviceState_appStarting:
				app_main();
				shared.deviceState = DeviceState_appRunning;
				break;
			
			case DeviceState_appRunning:
				app_main();
				break;
			
			case DeviceState_appShutdown:
				app_main();
				shared.deviceState = DeviceState_appStopped;
				break;
			
			case DeviceState_appCrcError:
			case DeviceState_unknown:
			case DeviceState_appStopped:
				break;
		}
		
		watchdogReset();
	}	
}

void device_eraseApp(void)
{
	if(shared.deviceState == DeviceState_appStarting || shared.deviceState == DeviceState_appRunning)
	{
		shared.deviceState = DeviceState_appShutdown;
		app_main();
	}
	
	shared.deviceState = DeviceState_appCrcError;
	bootloader_eraseAppSection();
}

void device_reboot(void)
{

#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
	RSTCTRL.SWRR = 0x01;
#endif

#ifdef ATTINYx41
	// NOTE: this is not resetting the peripherals
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
	#if defined(ATTINYx41) || defined(AVRxxEBxx)
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
#if defined(ATTINYx41) || defined(AVRxxEBxx) || defined(ATMEGAx8x)
	return bootloader_readEeprom(&eeSettings.deviceAddress);
#endif
}

com_baudRate_t device_getBaudRate(void)
{
#ifdef TINYAVR_1SERIES
	return bootloader_readEeprom((&eeSettings.baudRate)+EepromOffset);
#endif
#if defined(ATTINYx41) || defined(AVRxxEBxx) || defined(ATMEGAx8x)
	return bootloader_readEeprom(&eeSettings.baudRate);
#endif
}

void device_setBaudRate(uint8_t baudRateIndex)
{
	baudRate = baudRateIndex;
	com_setBaudrate(baudRate);
}

void device_saveBaudRate(void)
{
#if defined(TINYAVR_1SERIES) 
	return bootloader_updateEeprom((&eeSettings.baudRate)+EepromOffset, baudRate);
#endif
#if defined(ATTINYx41) || defined(AVRxxEBxx) || defined(ATMEGAx8x)
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

bool device_writeEepromAppSection(uint16_t offset, const uint8_t *data, uint16_t size)
{
	#define EepromAppSize (EepromSize-AppEepromStart)
	
	if(size+offset > EepromAppSize){
		return false;
	}
	
	#if defined(TINYAVR_1SERIES) || defined(AVRxxEBxx)
		
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