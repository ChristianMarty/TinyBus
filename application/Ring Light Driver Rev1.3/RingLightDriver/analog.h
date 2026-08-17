//**********************************************************************************************************************
// FileName : analog.h
// FilePath : /
// Project  : Ring Light Driver
// Author   : Christian Marty
// Date		: 17.08.2026
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef ANALOG_H_
#define ANALOG_H_

#include <avr/io.h>

void analog_init(void);
uint16_t analog_readVoltage(void);

#endif /* ANALOG_H_ */