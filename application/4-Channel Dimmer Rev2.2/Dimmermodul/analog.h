//**********************************************************************************************************************
// FileName : analog.h
// FilePath : /
// Project  : 4-Channel PWM Dimmer
// Author   : Christian Marty
// Date		: 08.05.2017
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef ANALOG_H_
#define ANALOG_H_

#include <stdint.h>

void analog_init(void);

uint16_t analog_readVoltage(void);
uint16_t analog_readCurrent(void);

#endif /* ANALOG_H_ */