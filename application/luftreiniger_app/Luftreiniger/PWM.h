/*
 * PWM.h
 *
 * Created: 20.05.2017 11:49:17
 *  Author: Christian
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

void pwm_init(void);
void pwm_update(uint8_t value);

#endif /* PWM_H_ */