//**********************************************************************************************************************
// FileName : tickTimer.h
// FilePath : common/
// Author   : Christian Marty
// Date		: 26.05.2024
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef TICK_TIMER_H_
#define TICK_TIMER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <main.h>

typedef struct {
	uint16_t counter;
} tickTimer_t;

void tickTimer_init(void);
bool tickTimer_delay5ms(tickTimer_t *tickTimer, uint16_t delay);
void tickTimer_reset(tickTimer_t *tickTimer);

#ifndef TEST_RUN
void tickTimer_interruptHandler(void) __attribute__ ((signal));
#else
void tickTimer_interruptHandler(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* TICK_TIMER_H_ */
