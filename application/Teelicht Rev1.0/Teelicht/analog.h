//**********************************************************************************************************************
// FileName : analog.h
// FilePath : /
// Author   : Christian Marty
// Date		: 21.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef ANALOG_H_
#define ANALOG_H_
#include <avr/io.h>

void analog_init(void);

uint16_t analog_read_voltage(void);
uint8_t analog_read_rand(void);

#endif /* ANALOG_H_ */