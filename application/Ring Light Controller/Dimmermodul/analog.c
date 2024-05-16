/*
 * analog.c
 *
 * Created: 23.06.2017 21:43:33
 *  Author: Christian
 */ 

#include "analog.h"

uint16_t adc_voltage = 0;
uint16_t adc_temperature = 0;

void analog_init(void)
{
	DIDR0 = 0x10; // Disable digital input buffer for ADC pins 
	ADCSRB = 0b00001000;
	ADCSRA = 0b10010100;
	
	adc_voltage = 0;
	adc_temperature = 0;
}

// Returns Voltage reading in 10mV 
uint16_t analog_read_voltage(void)
{	
	ADMUXB = 0x60; // Set reference to 4.096V and Gain to 1
	ADMUXA = 4; // Set ADC input MUX to Single ended ADC4

	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Conversion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	ADCSRA  |= 0x10; // Clear flag
	
	uint16_t ADC_measurement = 0;
	
	for(uint8_t i = 0 ; i<21; i++) // resistive divider is 1/21 so take and sum 21 samples
	{
		ADCSRA |= 0x40; // Start ADC Conversion
		while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish 
		
		ADC_measurement +=  ADCH;
		
		ADCSRA  |= 0x10; // Clear flag
	}
	ADC_measurement = ADC_measurement *16;   // convert result to 1mV resolution
	
	return ADC_measurement;
}