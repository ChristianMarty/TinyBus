//**********************************************************************************************************************
// FileName : protocol.c
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"
#include "protocol.h"
#include "com_uart.h"
#include "typedef.h"
#include "device.h"
#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TINYAVR_1SERIES
	#include <avr/pgmspace.h>
	#include "bootloader_1series.h"
#endif
#ifdef ATTINYx41
	#include <avr/pgmspace.h>
	#include "bootloader_x41.h"
#endif
#ifdef TEST_RUN
    #include "bootloader_test.h"
    void app_receiveDataHandler(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
    {}
#endif

#ifdef RxTxLedEnable
 extern int8_t rxLedTimer5ms;
#endif

typedef enum  {
	CMD_GET_DEVICE_STATE = 0,
	CMD_GET_HARDWARE_INFO,
	CMD_GET_MEMORY_INFO,
	CMD_GET_APP_CRC,
	CMD_ERASE_APP,
	CMD_WRITE_FLASH_PAGE,
	
	CMD_WRITE_EEPROM,
	CMD_READ_EEPROM,
	CMD_READ_RAM,
	
	CMD_REBOOT = 10,
	CMD_APP_START,
	CMD_APP_STOP,
	CMD_GET_APP_NAME,
	CMD_GET_APP_VERSION,
	CMD_SET_ADDRESS,
	
	CMD_UNDEFINED = 0xFF
}kernel_command_t;

#define BROADCAST_ADDRESS  0x0F

uint16_t unpackU16( uint8_t *data){
	return ((uint16_t)(data[0]<<8))+data[1];
}

//**************************************************************************  
//
//	Communication Handler -> RX Complete Call Back
//
//**************************************************************************  
void com_receiveData(uint8_t instruction_byte, uint8_t *data, uint8_t size)
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
		if(shared.deviceState == APP_RUNNING){
			app_receiveDataHandler(command, data, size, (des_address == BROADCAST_ADDRESS));
		}else if(shared.deviceState == APP_STOPPED){
			shared.deviceState = APP_CHECK_CRC;
		}
	}
	else// If kernel command is received
	{
		uint8_t acknowledgmentData[20];	
		kernel_command_t subcommand;
		if(size == 0){ // in case no sub-command was received
			subcommand = CMD_UNDEFINED;
		}else{
			subcommand = data[0];
			acknowledgmentData[0] = subcommand | 0x80;
		}
		
		uint8_t acknowledgmentSize = 1;
		uint8_t error = 0;
		
		// Decode kernel sub-command
		switch(subcommand)
		{
			case CMD_GET_DEVICE_STATE:{
				acknowledgmentData[1] = (shared.address<<4) | (uint8_t)shared.deviceState;
				acknowledgmentSize ++;
				break;
			}
			case CMD_GET_HARDWARE_INFO:{
				acknowledgmentData[1] = (CONTROLLER_ID>>8);
				acknowledgmentData[2] = (CONTROLLER_ID&0xff);
				
				acknowledgmentData[3] = (HARDWARE_ID>>8);
				acknowledgmentData[4] = (HARDWARE_ID&0xff);
				
				acknowledgmentData[5] = HARDWARE_VERSION_MAJOR;
				acknowledgmentData[6] = HARDWARE_VERSION_MINOR;
				
				acknowledgmentData[7] = KERNEL_VERSION_MAJOR;
				acknowledgmentData[8] = KERNEL_VERSION_MINOR;
				
				acknowledgmentSize += 8;

				break;		
			}
			case CMD_GET_MEMORY_INFO:{
				acknowledgmentData[1] = (FlashByteSize>>8);
				acknowledgmentData[2] = (FlashByteSize&0xff);
				
				acknowledgmentData[3] = (AppBaseByteAddress>>8);
				acknowledgmentData[4] = (AppBaseByteAddress&0xff);
								
				acknowledgmentData[5] = FlashPageByteSize;
				
				acknowledgmentData[6] = (RamSize>>8);
				acknowledgmentData[7] = (RamSize&0xff);
				
				acknowledgmentData[8] = (AppRamStart>>8);
				acknowledgmentData[9] = (AppRamStart&0xff);
				
				acknowledgmentData[10] = (EepromSize>>8);
				acknowledgmentData[11] = (EepromSize&0xff);
				
				acknowledgmentData[12] = (AppEepromStart>>8);
				acknowledgmentData[13] = (AppEepromStart&0xff);
				
				acknowledgmentSize += 13;
				
				break;
			}
			case CMD_GET_APP_CRC:{
				uint16_t crc = bootloader_appCRC();
				acknowledgmentData[2] = (crc &0x00FF);
				acknowledgmentData[1] = (crc >>8);
				if(shared.deviceState == APP_CRC_ERROR && bootloader_checkAppCRC(crc) == 0){
					shared.deviceState = APP_STOPPED;
				}
			
				acknowledgmentSize += 2;
				break;
			}
			case CMD_ERASE_APP:{
				device_eraseApp();
				break;
			}
			case CMD_WRITE_FLASH_PAGE:{
				uint16_t address = unpackU16(&data[1]);
				if((address >= AppBaseByteAddress)&&(address <= (FlashByteSize - BootloadTransmitChunkSize)+1)){ // Check if Address location is in application sector
					bootloader_writePage(address, &data[3]);
				}else{
					error++;
				}
				break;
		}
			
	#ifdef EEPROM_WRITE
			case CMD_WRITE_EEPROM:{
				uint16_t address = (uint16_t)(data[1]<<8);
				address |= (uint16_t)(data[2]);
				if(address >= AppEepromStart && address < EepromSize){
					address += EepromOffset;
					for(uint8_t i = 0; i<size-3; i++){
						bootloader_updateEeprom((uint8_t*)(address+i), data[i+3]);
					}
				}else{
					error ++;
				}
				break;
			}	
	#endif
	
	#ifdef EEPROM_READ
			case CMD_READ_EEPROM:{
				uint16_t address = unpackU16(&data[1]);
				uint8_t size = data[3];
				if(size>16 || address>EepromSize){
					error++;
				}else{
					address+=EepromOffset;
					for(uint8_t i = 0; i<size; i++){
						acknowledgmentData[i+1] = bootloader_readEeprom((uint8_t*)(address+i));
					}
					acknowledgmentSize+=size;
				}
				break;
			}
	#endif
	
	#ifdef RAM_READ
			case CMD_READ_RAM:{
				uint16_t address  = unpackU16(&data[1]);
				uint8_t size = data[3];
				if(size>16 || address>RamSize){
					error++;
				}else{
					uint8_t *ramData = ((uint8_t*)(RamOffset+address));
					for(uint8_t i = 0; i<size; i++){
						acknowledgmentData[i+1] = ramData[i];
					}
					acknowledgmentSize+=size;
				}
				break;
			}
	#endif

			case CMD_REBOOT:{
				device_reboot();
				break;
			}
			case CMD_APP_START:{
				shared.deviceState = APP_CHECK_CRC;
				break;
			}
			case CMD_APP_STOP:{
				if(shared.deviceState == APP_RUNNING || shared.deviceState == APP_START){
					shared.deviceState = APP_SHUTDOWN;
				}
				break;
			}
			case CMD_GET_APP_NAME:{
				if(shared.deviceState == APP_CRC_ERROR){
					error++;
					break;
				}
				for(uint8_t i = 0; i<18; i++){
					uint8_t byte = bootloader_readByte(AppBaseByteAddress+14+i);
					if(byte == 0x00 || byte == 0xff) break;
					acknowledgmentData[i+1] = byte;
					acknowledgmentSize++;
				}
				break;
			}
			case CMD_GET_APP_VERSION:{
				if(shared.deviceState == APP_CRC_ERROR){
					error++;
					break;
				}
				acknowledgmentData[1] = bootloader_readByte(AppBaseByteAddress+2);
				acknowledgmentData[2] = bootloader_readByte(AppBaseByteAddress+3);
				acknowledgmentSize += 2;
				break;
			}
			case CMD_SET_ADDRESS:{
				if(!device_updateAddress(data[1])) error ++;
				break;
			}
			default:{
				error++;
				break;
			}
		}
		com_transmitData(instruction_byte, &acknowledgmentData[0], acknowledgmentSize, error);
	}
}
