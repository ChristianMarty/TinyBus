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
		uint8_t Ack_data[20];
		Ack_data[0] = subcommand | 0x80;
		uint8_t Ack_size = 1;
		uint8_t cmd_error = 0;
		
		if(size == 0) // in case no sub-command was received
		{
			cmd_error++;
			subcommand = 0xFF;
		}
			
		// Decode kernel sub-command
		switch(subcommand)
		{
			case CMD_GET_DEVICE_STATE:
					Ack_data[1] = (shared.address<<4) | (uint8_t)shared.deviceState;
					Ack_size ++;
					break;
				
			case CMD_GET_DEVICE_INFO:
					Ack_data[1] = (shared.address<<4) | (uint8_t)shared.deviceState;
					Ack_data[2] = KERNEL_REVISION;
					Ack_data[3] = CONTROLLER_ID;
					Ack_data[4] = DEVICE_ID;
					Ack_data[5] = DEVICE_HW_REV;
				
					Ack_data[6] = (AppBaseByteAddress>>8);
					Ack_data[7] = (AppBaseByteAddress&0xff);
				
					Ack_data[8] = (AppSize>>8);
					Ack_data[9] = (AppSize&0xff);
				
					Ack_data[10] = (AppEEPROMAddress>>8);
					Ack_data[11] = (AppEEPROMAddress&0xff);
				
					Ack_data[12] = (AppEepromSize>>8);
					Ack_data[13] = (AppEepromSize&0xff);
				
					Ack_data[14] = (AppRamAddress>>8);
					Ack_data[15] = (AppRamAddress&0xff);
				
					Ack_data[16] = (AppRamSize>>8);
					Ack_data[17] = (AppRamSize&0xff);
				
					Ack_data[18] = FlashPageByteSize;
					Ack_size += 18;
					break;
				
			case CMD_GET_APP_CRC:
					u16_temp = bootloader_appCRC();
					Ack_data[2] = (u16_temp &0x00FF);
					Ack_data[1] = (u16_temp >>8);
					if(shared.deviceState == APP_CRC_ERROR && bootloader_checkAppCRC(u16_temp) == 0)
					{
						shared.deviceState = APP_STOPPED;
					}
			
					Ack_size += 2;
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
						cmd_error ++;
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

			case CMD_SET_ADDRESS:
					if(!device_updateAddress(data[1])) cmd_error ++;
					break;
		}
		com_transmit_data(instruction_byte,&Ack_data[0],Ack_size,cmd_error);
	}
}


