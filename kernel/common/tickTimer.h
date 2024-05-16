//**********************************************************************************************************************
// FileName : tickTimer.h
// FilePath : /
// Author   : Christian Marty
// Date		: 27.05.2023
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef TICKTIMER_H_
#define TICKTIMER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <main.h>

typedef uint16_t tickTimer_t;

void tickTimer_init(void);
bool tickTimer_delay5ms(tickTimer_t *counter, uint16_t delay);
void tickTimer_reset(tickTimer_t *counter);
#ifndef TEST_RUN
void tickTimer_interruptHandler(void) __attribute__ ((signal));
#else
void tickTimer_interruptHandler(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* TICKTIMER_H_ */
