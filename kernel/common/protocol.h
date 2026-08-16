//**********************************************************************************************************************
// FileName : protocol.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#ifndef TEST_RUN
#include <avr/io.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

void com_receiveData(uint8_t instructionByte, const uint8_t *data, uint8_t size);

#ifdef __cplusplus
}
#endif
#endif /* PROTOCOL_H_ */