//**********************************************************************************************************************
// FileName : device.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "device.h"
#include "typedef.h"
#include "main.h"
#include <avr/pgmspace.h>

#ifdef TINYAVR_1SERIES
	#include "bootloader_1series.h"
#endif
#ifdef ATTINYx41
	#include "bootloader_x41.h"
#endif

uint8_t eeDeviceAddress EEMEM = 0;
shared_t shared __attribute__((section (".shared")));

uint8_t device_getAddress(void);

void device_init(void)
{
	shared.address = device_getAddress();
	if(shared.address >= 0xF) shared.address = 0x00; // in case the EEPROM was never programmed or address is out of range (>15)
	
	shared.deviceState = APP_STOPPED;
	shared.appCrc = bootloader_appCRC();
	if(bootloader_checkAppCRC(shared.appCrc) != 0) shared.deviceState = APP_CRC_ERROR;
	
	sei();
	tickTimer_init();
	com_init();
	
	if(shared.deviceState == APP_STOPPED && (MCUSR | 0x08) == false) //only in case watchdog reset was not triggered
	{
		uint8_t byte = pgm_read_byte(AppBaseByteAddress);
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

