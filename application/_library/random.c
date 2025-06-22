//**********************************************************************************************************************
// FileName : random.c
// FilePath : /
// Author   : Christian Marty
// Date		: 21.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "random.h"

#ifdef __cplusplus
	extern "C" {
#endif

//Linear-feedback shift register
uint16_t random_number = 0;

void random_initialize(uint16_t seed)
{
	random_number = seed;
}

uint16_t random_next(uint16_t n)
{
	return (n >> 0x01) ^ (-(n & 0x01) & 0xB400);
}

uint16_t random_getNumber(void)
{
	random_number = random_next(random_number);
	return random_number;
}


#ifdef __cplusplus
	}
#endif