//**********************************************************************************************************************
// FileName : protocol.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef TEST_RUN
#include <avr/io.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

void com_receiveData(uint8_t instruction_byte, uint8_t *data, uint8_t size);

#ifdef __cplusplus
}
#endif