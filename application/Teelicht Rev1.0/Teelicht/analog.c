//**********************************************************************************************************************
// FileName : analog.c
// FilePath : /
// Author   : Christian Marty
// Date		: 21.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "analog.h"

void analog_init(void)
{
	DIDR0 = 0x01; // Disable digital input buffer for ADC pins 
	ADCSRB = 0;
	ADCSRA = 0b10010011;
}

// Returns Voltage reading in 1mV 
uint16_t analog_read_voltage(void)
{	
	ADMUXB = 0x60; // Set reference to 4.096V and Gain to 1
	ADMUXA = 0x00; // Set ADC input MUX to Single ended ADC0

	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Conversion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	ADCSRA  |= 0x10; // Clear flag
	
	uint16_t measurement = 0;
	
	for(uint8_t i = 0; i<11; i++) // voltage-sensing is 1/11
	{
		ADCSRA |= 0x40; // Start ADC Conversion
		while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish 
		
		uint16_t value = ADCL & 0x00FF;
		value |= (( (uint16_t)ADCH <<8) & 0x0300); 
		
		ADCSRA  |= 0x10; // Clear flag
		
		measurement += value;
	}
	
	measurement = measurement * 4;
	
	return measurement;
}

uint8_t analog_read_rand(void)
{
	ADMUXB = 0x00; // Set reference to VCC and Gain to 1
	ADMUXA = 0x0C; // Set ADC input MUX to Single ended Temperature sensor
	
	//ADMUXB = 0x60; // Set reference to 4.096V and Gain to 1
	//ADMUXA = 0x00; // Set ADC input MUX to Single ended ADC0

	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Conversion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	ADCSRA  |= 0x10; // Clear flag
	
	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Conversion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	
	uint16_t value = ADCL & 0x00FF;
	value |= (( (uint16_t)ADCH <<8) & 0x0300);
	
	ADCSRA  |= 0x10; // Clear flag
	
	return (uint8_t)value;
}
