/*
 * Not-Aus.c
 *
 * Created: 22.04.2026 21:12:04
 * Author : Christian
 */ 
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "sharedFunctions.h"
#include "typedef.h"

#define MAJOR_SW_REV 1
#define MINOR_SW_REV 1

#define MAJOR_HW_REV 1
#define MINOR_HW_REV 4

#define HARDWARE_ID 0x0005

#define APPLICATION_NAME "Not-Aus Relais" // Max 18 characters

volatile shared_t shared __attribute__((section (".shared")));
volatile const application_header_t header __attribute__((section (".header"))) = {
	.autostart = true,
	.header_version = 0,
	.firmwareVersion_major = MAJOR_SW_REV,
	.firmwareVersion_minor = MINOR_SW_REV,
	.hardwareId_h = (uint8_t)(HARDWARE_ID>>8),
	.hardwareId_l = (uint8_t)(HARDWARE_ID),
	.name = APPLICATION_NAME
};


#define HEARTBEAT_LED_ON() PORTA.OUTSET = 0x10
#define HEARTBEAT_LED_OFF() PORTA.OUTCLR = 0x10

typedef union {
	struct {
		uint8_t switch1 :1;
		uint8_t switch2 :1;
		uint8_t switch3 :1;
		uint8_t switch4 :1;
		
		uint8_t relaisA :1;
		uint8_t relaisB :1;
		
		uint8_t reset :1;
		
		uint8_t reserved :1;
	}bit;
	uint8_t byte;
} Input;

typedef struct {
	struct {
		uint8_t on  :1;
		uint8_t off :1;
		uint8_t relaisA :1;
		uint8_t relaisB :1;
		
		uint8_t reserved :4;
	}bit;
	uint8_t byte;
} Output;

typedef struct {
	uint16_t vIn;
	uint16_t v5v;
	uint16_t vVdd;
	int16_t vTemperature;
} AnalogReadings;

typedef enum {
	State_Startup,
	State_Ready,
	State_Run,
	State_Error
} State;
State systemState;

volatile uint8_t taskTimer = 0;
Output outputState;
Input inputState;
AnalogReadings readings;

Input readInput(void);

void runLoop(State* systemState, Input *input, Output *output);
void writeOutput(Output output);

void runHeartbeat(void);

void adc_init(void);
void adc_run(void);


void app_main(void)
{
	
	if(shared.deviceState == APP_START)
	{
		inputState.byte = 0;
		outputState.byte = 0;
		systemState = State_Startup;
		adc_init();
	}

	if(taskTimer > 20) // run every 100ms
	{
		taskTimer = 0;
		
		runHeartbeat();
		inputState = readInput();
		runLoop(&systemState, &inputState, &outputState);
		writeOutput(outputState);
	}
	
	adc_run();
	
	if(shared.deviceState == APP_SHUTDOWN)
	{
		HEARTBEAT_LED_OFF();
		outputState.bit.on = false;
		outputState.bit.off = false;
		outputState.bit.relaisA = false;
		outputState.bit.relaisB = false;
		writeOutput(outputState);
	}
}


// Com RX Complete Call Back
void app_com_receive_data(uint8_t instruction, uint8_t *data, uint8_t size, bool broadcast)
{
	uint8_t ack_data[10];
	uint8_t cmd_error = 0;
	uint8_t ack_size = 0;
	
	switch(instruction)
	{
		case 0: {
			ack_data[0] = inputState.byte;
			ack_data[1] = outputState.byte;
			
			ack_data[2] = readings.vIn & 0xFF;
			ack_data[3] = (readings.vIn>>8) & 0xFF;
			
			ack_data[4] = readings.v5v & 0xFF;
			ack_data[5] = (readings.v5v>>8) & 0xFF;
			
			ack_data[6] = readings.vVdd & 0xFF;
			ack_data[7] = (readings.vVdd>>8) & 0xFF;
			
			ack_data[8] = readings.vTemperature & 0xFF;
			ack_data[9] = (readings.vTemperature>>8) & 0xFF;
			
			ack_size = 10;
			break;
		}
		
		// Command not found
		default:
		cmd_error++;
		break;
	}
	
	if(!broadcast){
		com_transmit_data(instruction, &ack_data[0], ack_size, cmd_error);
	}
}

void app_5ms_tick(void)
{
	taskTimer++;
}

void runHeartbeat(void)
{	
	static uint8_t hertbeatTimer = 0;
	
	if(hertbeatTimer == 0 || hertbeatTimer == 2){
		HEARTBEAT_LED_ON();
	}else if(hertbeatTimer == 1 || hertbeatTimer == 3){
		HEARTBEAT_LED_OFF();
	}
	
	hertbeatTimer++;
	if(hertbeatTimer > 10){
		hertbeatTimer = 0;
	}
}

void runLoop(State* state, Input *input, Output *output)
{
	if(systemState == State_Startup){
		output->bit.on = false;
		output->bit.off = false;
		output->bit.relaisA = false;
		output->bit.relaisB = false;
		
		// In case one relays is not off at startup -> assume that the relays is broken
		if(input->bit.relaisA || input->bit.relaisB){
			systemState = State_Error;
		}else{	
			systemState = State_Ready;
		}
		return;
	}
	
	
	if(systemState == State_Ready){
		if(input->bit.reset){
			output->bit.relaisA = true;
			output->bit.relaisB = true;
			systemState = State_Run;
		}
		return;
	}
	
	
	if(systemState == State_Run){
		
		if(input->bit.relaisA != input->bit.relaisB){
			systemState = State_Error;
		}
		
		output->bit.on = input->bit.relaisA && input->bit.relaisB;
		output->bit.off = !output->bit.on;
		
		return;
	}
	
	if(systemState == State_Error){
		output->bit.relaisA = false;
		output->bit.relaisB = false;
		return;
	}	
}

Input readInput(void){
	Input input;
	
	input.bit.relaisA = !(bool)(PORTD.IN & 0x02);
	input.bit.relaisB = !(bool)(PORTD.IN & 0x04);
	
	input.bit.switch1 = !(bool)(PORTA.IN & 0x80);
	input.bit.switch2 = !(bool)(PORTC.IN & 0x01);
	input.bit.switch3 = !(bool)(PORTD.IN & 0x08);
	input.bit.switch4 = !(bool)(PORTF.IN & 0x10);
	
	input.bit.reset = !(bool)(PORTF.IN & 0x20);
	
	return input;
}

void writeOutput(Output output)
{
	if(output.bit.off) PORTA.OUTSET = 0x40;
	else PORTA.OUTCLR = 0x40;
	
	if(output.bit.on) PORTA.OUTSET = 0x20;
	else PORTA.OUTCLR = 0x20;
	
	if(output.bit.relaisA) PORTF.OUTSET = 0x04;
	else PORTF.OUTCLR = 0x04;
	
	if(output.bit.relaisB) PORTF.OUTSET = 0x08;
	else PORTF.OUTCLR = 0x08;
}

#define ADC_24V 0x1F
#define ADC_5V 0x00
#define ADC_VDD 0x31
#define ADC_TEMPERATURE 0x32

void adc_init(void)
{
	ADC0.MUXNEG = 0x30; //  Negative Input -> GND
	
	ADC0.CTRLF = 0x03; // 8 Samples
	ADC0.CTRLC = 0x06; // Internal reference 4.096V
	ADC0.CTRLA = 0x01; // Enable
}

void adc_read(uint8_t source)
{
	ADC0.MUXPOS = source;
	ADC0.COMMAND = 0x41;
}

void adc_run(void)
{
	if(ADC0.STATUS){
		return; // if ADC  busy -> return
	}
	
	static uint8_t adcReadingIndex = 0;
	switch(adcReadingIndex){
		
		case 0:{
			readings.vIn = (ADC0.RESULT>>3)*11;
			adc_read(ADC_5V);
			break;
		}
		
		case 1:{
			readings.v5v = (ADC0.RESULT>>2);
			adc_read(ADC_VDD);
			break;
		}
		
		case 2:{
			readings.vVdd = (ADC0.RESULT>>3)*10;
			adc_read(ADC_TEMPERATURE);
			break;
		}
		
		case 3:{
			
			#define SCALING_FACTOR 4096 // Enables integer in the signature row
			int16_t sigrow_offset = (int16_t) SIGROW.TEMPSENSE1; // Read signed offset from signature row
			int16_t sigrow_slope = (int16_t) SIGROW.TEMPSENSE0; // Read signed slope from signature row
			uint16_t adc_reading = ADC0.RESULT; // ADC conversion result
			
			int32_t temp = ((int32_t) adc_reading) + sigrow_offset;
			temp *= sigrow_slope; // Result can overflow 16-bit variable
			temp += SCALING_FACTOR / 2; // Ensures correct rounding on division below
			temp /= SCALING_FACTOR; // Round to the nearest integer in Kelvin
			readings.vTemperature = temp - 273;
			
			adc_read(ADC_24V);
			break;
		}
	}
	
	adcReadingIndex++;
	if(adcReadingIndex >= 4){
		adcReadingIndex = 0;
	}
}
