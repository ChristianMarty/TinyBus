/*
 * main_driver.c
 *
 * Created: 05.06.2017 23:21:19
 *  Author: Christian
 */ 
#include "main_driver.h"
#include <avr/eeprom.h>


uint8_t eeDeviceAddress EEMEM = 0;


void device_init(void)
{
	
	
}

void device_set_address(uint8_t new_address)
{
	eeprom_write_byte(&eeDeviceAddress, new_address);
}

uint8_t device_get_address(void)
{
	return eeprom_read_byte (&eeDeviceAddress);
}