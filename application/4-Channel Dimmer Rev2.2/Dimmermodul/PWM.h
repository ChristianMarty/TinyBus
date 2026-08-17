//**********************************************************************************************************************
// FileName : pwm.h
// FilePath : /
// Project  : 4-Channel PWM Dimmer
// Author   : Christian Marty
// Date		: 08.05.2017
// Website  : www.christian-marty.ch
//**********************************************************************************************************************
#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>

void pwm_init(void);

void pwm_setChannel1(uint16_t value);
void pwm_setChannel2(uint16_t value);
void pwm_setChannel3(uint16_t value);
void pwm_setChannel4(uint16_t value);

void pwm_fade(uint8_t channel, uint8_t fadetime, uint16_t value, bool waitForExecution);
void pwm_set(uint8_t channel ,uint16_t value);

void pwm_5ms_tick(void);

void pwm_executeAll(void);
void pwm_execute(uint8_t channel);

#endif /* PWM_H_ */