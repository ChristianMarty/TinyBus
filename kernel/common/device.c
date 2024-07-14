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
    uint8_t eeDeviceAddress __attribute__((section(".eeprom"))) = 0;
#endif
#ifdef ATTINYx41
	#include "bootloader_x41.h"
    uint8_t eeDeviceAddress __attribute__((section(".eeprom"))) = 0;
#endif
#ifdef TEST_RUN
    #include "bootloader_test.h"
    uint8_t eeDeviceAddress = 0;
    void app_main()
    {}
#endif

shared_t shared __attribute__((section (".shared")));

uint8_t device_getAddress(void);

void device_init(void)
{
	shared.address = device_getAddress();
	if(shared.address >= 0xF) shared.address = 0x00; // in case the EEPROM was never programmed or address is out of range (>15)
	
	shared.deviceState = APP_STOPPED;
	shared.appCrc = bootloader_appCRC();
	if(bootloader_checkAppCRC(shared.appCrc) != 0){
		shared.deviceState = APP_CRC_ERROR;
	}
	sei();
	tickTimer_init();
	com_init();
	
#ifdef TINYAVR_1SERIES
	uint8_t watchdogReset = (RSTCTRL.RSTFR | 0x08);
#endif
#ifdef ATTINYx41
	uint8_t watchdogReset = (MCUSR | 0x08);
#endif
#ifdef TEST_RUN
    uint8_t watchdogReset = false;
#endif

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
		bootloader_updateEeprom(&eeDeviceAddress, address);
		if(address == device_getAddress()){
			return true;
		}
	}
	return false;
}

uint8_t device_getAddress(void)
{
	return bootloader_readEeprom(&eeDeviceAddress);
}

#ifdef __cplusplus
}
#endif