//**********************************************************************************************************************
// FileName : protocol.h
// FilePath : /common
// Author   : Christian Marty
// Date		: 27.05.2023
// Website  : www.christian-marty.ch
//**********************************************************************************************************************

#include <avr/io.h>

void com_receive_data(uint8_t instruction_byte, uint8_t *data, uint8_t size);
