/*
 * PWM.h
 *
 * Created: 20.05.2017 11:49:17
 *  Author: Christian
 */ 
#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>
#include <stdbool.h>

void pwm_init(void);

void pwm_setWw(uint16_t pwm, uint8_t analog);
void pwm_setCw(uint16_t pwm, uint8_t analog);

#endif /* PWM_H_ */