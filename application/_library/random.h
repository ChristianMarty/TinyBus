//**********************************************************************************************************************
// FileName : random.h
// FilePath : /
// Author   : Christian Marty
// Date		: 21.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

void random_initialize(uint16_t seed);
uint16_t random_getNumber(void);

#ifdef __cplusplus
	}
#endif

#endif /* RANDOM_H_ */