/*
 * analog.h
 *
 * Created: 23.06.2017 21:43:52
 *  Author: Christian
 */ 

#ifndef ANALOG_H_
#define ANALOG_H_
#include <avr/io.h>

void analog_init(void);

uint16_t analog_read_voltage(void);
uint16_t analog_read_current(void);

#endif /* ANALOG_H_ */