//**********************************************************************************************************************
// FileName : main.c
// FilePath : /
// Project  : PAR-16
// Author   : Christian Marty
// Date		: 22.10.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "main.h"
#include <avr/io.h>

#include "sharedFunctions.h"
#include "typeDefinition.h"

#include "PWM.h"
#include "analog.h"

volatile shared_t shared __attribute__((section (".shared")));
volatile const applicationHeader_t header __attribute__((section (".header"))) = {
	.autostart = true,
	.headerVersion = 0,
	.firmwareVersion_major = MAJOR_SW_REV,
	.firmwareVersion_minor = MINOR_SW_REV,
	.hardwareId_h = (uint8_t)(HARDWARE_ID>>8),
	.hardwareId_l = (uint8_t)(HARDWARE_ID),
	.name = APPLICATION_NAME
};

void cmd_setPwm(uint8_t pwm_ch, uint8_t *data);
void updateLight();

uint16_t brightness;
uint8_t temperature;

void app_main(void)
{
	if(shared.deviceState == DeviceState_appStarting)
	{	
		pwm_setWw(0x00, 0x00);
		pwm_setCw(0x00, 0x00);
		pwm_init();
	
		PORTA &= 0x87;
		DDRA = 0xFC;
		DDRB = 0x00;
		PORTA |= 0x80;
	
		analog_init();
		
		if(shared.carrierDetected == false){
			readEepromAppSection(0, &temperature, 1);
			readEepromAppSection(1, (uint8_t*)&brightness, 2);
		}
		
		updateLight();
	}
	
	// Add main code here
	
	if(shared.deviceState == DeviceState_appShutdown)
	{
		// Turn LED off
		pwm_setWw(0x00, 0x00);
		pwm_setCw(0x00, 0x00);
	}
}

// Com RX Complete Call Back
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
	uint8_t ack_data[4];
	uint8_t cmd_error = 0;
	uint8_t ack_size = 0;
		
	switch(instruction)
	{
		case 0:
			if(size == 2){
				brightness = (data[0]<<8) | data[1];
				updateLight();
			}else{
				cmd_error++;
			}
			break;
			
		case 1: 
			if(size == 1){
				temperature = data[0];
				updateLight();
			}else{
				cmd_error++;
			}
			break;
			
		
		case 2:
			if(size == 3){
				brightness = (data[0]<<8) | data[1];
				temperature = data[2];
				updateLight();
			}else{
				cmd_error++;
			}
			break;
			
			
		// Set raw pwm
		case 4:	cmd_setPwm(0, data);
				cmd_setPwm(1, data+4);
				break;
		
		// set default value 
		case 8:	{
			uint16_t offset = (data[0]<<8) | data[1];
			if(!writeEepromAppSection(offset, &data[2], size-2)){
				cmd_error++;
			}
			break;
		}
		case 9:	{
			uint16_t offset = (data[0]<<8) | data[1];
			uint16_t size = (data[2]<<8) | data[3];
			if(!readEepromAppSection(offset, &ack_data[0], size)){
				cmd_error++;
			}
			ack_size+= size;
			break;
		}
		
		// Voltage / Current
		case 14: {
				uint16_t v_read = analog_readVoltage();
				uint16_t i_read = analog_readCurrent();
				
				// Returns the Voltage reading in 10mV and current reading in 1mA steps
				ack_data[0] = (uint8_t) (v_read>>8);
				ack_data[1] = (uint8_t) (v_read & 0x00FF);
				ack_data[2] = (uint8_t) (i_read>>8);
				ack_data[3] = (uint8_t) (i_read & 0x00FF);
				ack_size = 4;
				break;
		}
						
		// Command not found
		default:
				cmd_error++;
				break;
	}
		
	if(!broadcast) com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
}

// temperature 0 => cold, 255 => warm
void updateLight(void)
{
	if(brightness == 0){
		pwm_setCw(0, 0);
		pwm_setWw(0, 0);
		return;
	}
	
	uint32_t warm = ((uint32_t)brightness * temperature);
	uint32_t cold = ((uint32_t)brightness * (0xFF-temperature));
	
	pwm_setWw((warm>>8) & 0xFFFF, 0xFF);	
	pwm_setCw((cold>>8) & 0xFFFF, 0xFF);		
}

void cmd_setPwm(uint8_t pwm_ch, uint8_t *data)
{
	uint16_t pwm_data = (data[0]<<8);
	pwm_data |= data[1];
	
	uint16_t analog_data = (data[2]<<8);
	analog_data |= data[3];
	
	if(pwm_ch){
		pwm_setCw(pwm_data, analog_data&0xFF);
	}else{
		pwm_setWw(pwm_data, analog_data&0xfF);
	} 
}

void app_5ms_tick(void)
{

}






