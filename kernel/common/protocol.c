//**********************************************************************************************************************
// FileName : protocol.h
// FilePath : /common
// Author   : Christian Marty
// Date		: 27.05.2023
// Website  : www.christian-marty.ch
//**********************************************************************************************************************

#include "main.h"
#include "protocol.h"
#include "com_uart.h"
#include "typedef.h"
#include "device.h"

#include <avr/pgmspace.h>

#ifdef TINYAVR_1SERIES
	#include "bootloader_1series.h"
#endif
#ifdef ATTINYx41
	#include "bootloader_x41.h"
#endif

#ifdef RxTxLedEnable
 extern int8_t rxLedTimer5ms;
#endif

typedef enum  {
	CMD_GET_DEVICE_STATE = 0,
	CMD_GET_DEVICE_INFO,
	CMD_GET_APP_CRC,
	CMD_ERASE_APP,
	CMD_WRITE_PAGE,
	
	CMD_REBOOT = 0x08,
	CMD_APP_START,
	CMD_APP_STOP,
	CMD_GET_APP_NAME,
	CMD_GET_APP_VERSION,
	
	CMD_SET_ADDRESS = 15
}kernel_command_t;

//**************************************************************************  
//
//	Communication Handler -> RX Complete Call Back
//
//**************************************************************************  
void com_receive_data(uint8_t instruction_byte, uint8_t *data, uint8_t size)
{
	// Extract destination address from received data
	uint8_t des_address = (instruction_byte >> 4);
		
	// Extract command from received data
	uint8_t command = (instruction_byte & 0x0F);
	
	if(des_address != shared.address && des_address != BROADCAST_ADDRESS) return;
	
	#ifdef RxTxLedEnable
		rxLedTimer5ms = 0;
	#endif
		
	if(command != 15)
	{
		if(shared.deviceState == APP_RUNNING)
		{
			app_com_receive_data(command, data, size, (des_address == BROADCAST_ADDRESS));
		}
		else if(shared.deviceState == APP_STOPPED)
		{
			shared.deviceState = APP_CHECK_CRC;
		}
	}
	else// If kernel command is received
	{
				
		kernel_command_t subcommand = data[0];
		uint16_t u16_temp = 0;
		uint8_t acknowledgmentData[20];
		acknowledgmentData[0] = subcommand | 0x80;
		uint8_t acknowledgmentSize = 1;
		uint8_t error = 0;
		
		if(size == 0) // in case no sub-command was received
		{
			error++;
			subcommand = 0xFF;
		}
			
		// Decode kernel sub-command
		switch(subcommand)
		{
			case CMD_GET_DEVICE_STATE:
					acknowledgmentData[1] = (shared.address<<4) | (uint8_t)shared.deviceState;
					acknowledgmentSize ++;
					break;
				
			case CMD_GET_DEVICE_INFO:
					acknowledgmentData[1] = CONTROLLER_ID;
					acknowledgmentData[2] = (HARDWARE_ID>>8);
					acknowledgmentData[3] = (HARDWARE_ID&0xff);
					
					acknowledgmentData[4] = HARDWARE_VERSION_MAJOR;
					acknowledgmentData[5] = HARDWARE_VERSION_MINOR;
					
					acknowledgmentData[6] = KERNEL_VERSION_MAJOR;
					acknowledgmentData[7] = KERNEL_VERSION_MINOR;
					
					acknowledgmentData[8] = (AppBaseByteAddress>>8);
					acknowledgmentData[9] = (AppBaseByteAddress&0xff);
				
					acknowledgmentData[10] = (AppSize>>8);
					acknowledgmentData[11] = (AppSize&0xff);
				
					acknowledgmentData[12] = FlashPageByteSize;
					
					acknowledgmentSize += 12;
					break;
				
			case CMD_GET_APP_CRC:
					u16_temp = bootloader_appCRC();
					acknowledgmentData[2] = (u16_temp &0x00FF);
					acknowledgmentData[1] = (u16_temp >>8);
					if(shared.deviceState == APP_CRC_ERROR && bootloader_checkAppCRC(u16_temp) == 0)
					{
						shared.deviceState = APP_STOPPED;
					}
			
					acknowledgmentSize += 2;
					break;
								
			case CMD_ERASE_APP: 
					device_eraseApp();
					break;
						
			case CMD_WRITE_PAGE:
					u16_temp = (uint16_t)(data[1]<<8);
					u16_temp |= (uint16_t)(data[2]);
				
					if((u16_temp >= AppBaseByteAddress)&&(u16_temp <= (FlashByteSize - BootloadTransmitChunkSize)+1)) // Check if Address location is in application sector
					{
						bootloader_writePage(u16_temp, &data[3]);
					}
					else
					{
						error ++;
					}
					break;


			case CMD_REBOOT:
					device_reboot();
					break;
						
			case CMD_APP_START:
					shared.deviceState = APP_CHECK_CRC;
					break;
					
			case CMD_APP_STOP:
					if(shared.deviceState == APP_RUNNING || shared.deviceState == APP_START)
					{
						shared.deviceState = APP_SHUTDOWN;
					}
					break;
					
			case CMD_GET_APP_NAME:
					if(shared.deviceState == APP_CRC_ERROR){
						error++;
						break;
					}
					for(uint8_t i = 0; i<18; i++){
						uint8_t byte = pgm_read_byte(AppBaseByteAddress+14+i);
						if(byte == 0x00 || byte == 0xff) break;
						acknowledgmentData[i+1] = byte;
						acknowledgmentSize++;
					}
					break;
					
			case CMD_GET_APP_VERSION:
					if(shared.deviceState == APP_CRC_ERROR){
						error++;
						break;
					}
					acknowledgmentData[1] = pgm_read_byte(AppBaseByteAddress+2);
					acknowledgmentData[2] = pgm_read_byte(AppBaseByteAddress+3);
					acknowledgmentSize += 2;
					break;

			case CMD_SET_ADDRESS:
					if(!device_updateAddress(data[1])) error ++;
					break;
		}
		com_transmit_data(instruction_byte, &acknowledgmentData[0], acknowledgmentSize, error);
	}
}


