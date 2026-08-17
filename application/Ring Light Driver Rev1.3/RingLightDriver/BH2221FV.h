//**********************************************************************************************************************
// FileName : BH2221FV.h
// FilePath : /
// Project  : Ring Light Driver
// Author   : Christian Marty
// Date		: 17.08.2026
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef BH2221FV_H_
#define BH2221FV_H_

#include <avr/io.h>

void BH2221FV_sendAll(const uint8_t *data);
void BH2221FV_send(uint8_t channel, uint8_t data);

#endif /* BH2221FV_H_ */



