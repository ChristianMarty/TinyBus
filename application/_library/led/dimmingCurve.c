//**********************************************************************************************************************
// FileName : dimmingCurve.c
// FilePath : /utility
// Author   : Christian Marty
// Date		: 21.06.2025
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#include "dimmingCurve.h"

#include <avr/pgmspace.h>

#define DIMMING_CURVE_SIZE 64

const PROGMEM uint16_t dimmingCurve[DIMMING_CURVE_SIZE] = {
	0	,
	1	,
	2	,
	3	,
	4	,
	5	,
	6	,
	7	,
	8	,
	9	,
	10	,
	11	,
	12	,
	13	,
	14	,
	16	,
	18	,
	22	,
	25	,
	30	,
	34	,
	39	,
	44	,
	50	,
	57	,
	64	,
	72	,
	81	,
	90	,
	100	,
	111	,
	123	,
	135	,
	148	,
	161	,
	176	,
	192	,
	208	,
	225	,
	243	,
	263	,
	283	,
	304	,
	326	,
	349	,
	374	,
	399	,
	426	,
	453	,
	482	,
	512	,
	544	,
	576	,
	610	,
	645	,
	682	,
	720	,
	759	,
	800	,
	842	,
	885	,
	930	,
	977	,
	1024
};



uint16_t dimmingCurve_getValue(uint8_t index)
{
	
	return pgm_read_word(&dimmingCurve[index]);
}