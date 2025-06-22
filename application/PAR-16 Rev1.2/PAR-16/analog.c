/*
 * analog.c
 *
 * Created: 23.06.2017 21:43:33
 *  Author: Christian
 */ 

#include "analog.h"

void analog_init(void)
{
	DIDR0 = 0x03; // Disable digital input buffer for ADC pins 
	ADCSRA = 0b10010111;
}

// Returns Voltage reading in 10mV 
uint16_t analog_read_voltage(void)
{	
	ADMUXB = 0x60; // Set reference to 4.096V and Gain to 1
	ADMUXA = 0x01; // Set ADC input MUX to Single ended ADC1

	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Convertion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	ADCSRA  |= 0x10; // Clear flag
	
	uint16_t ADC_measurement = 0;
	
	for(uint8_t i = 0 ; i<12; i++)
	{
		ADCSRA |= 0x40; // Start ADC Convertion
		while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish 
		
		ADC_measurement += (uint16_t) ADCL;
		ADC_measurement += (uint16_t)(ADCH<<8);
		
		ADCSRA  |= 0x10; // Clear flag
	}
	ADC_measurement = ADC_measurement *4;
	ADC_measurement = (ADC_measurement /10);	
	
	return ADC_measurement;
}

// Returns Current reading in 1mA
uint16_t analog_read_current(void)
{
	ADMUXB = 0x61; // Set reference to 4.096V and Gain to 20
	ADMUXA = 0x30; // Set ADC input MUX to Differential ended n ADC0 p ADC1
	
	// Dummy Conversion
	ADCSRA |= 0x40; // Start ADC Convertion
	while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
	ADCSRA  |= 0x10; // Clear flag
	
	uint16_t ADC_measurement = 0;
	
	for(uint8_t i = 0 ; i<12; i++)
	{
		ADCSRA |= 0x40; // Start ADC Convertion
		while((ADCSRA&0x10) == 0); // Wait for ADC Conversion to finish
		
		ADC_measurement += (uint16_t) ADCL;
		ADC_measurement += (uint16_t)(ADCH<<8);
		
		ADCSRA  |= 0x10; // Clear flag
	}
	ADC_measurement = ADC_measurement *4;
	ADC_measurement = (ADC_measurement /2);
	
	return ADC_measurement;
}
