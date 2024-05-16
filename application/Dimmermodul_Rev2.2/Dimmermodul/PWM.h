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

void pwm_set_ch1(uint16_t pwm_value);
void pwm_set_ch2(uint16_t pwm_value);
void pwm_set_ch3(uint16_t pwm_value);
void pwm_set_ch4(uint16_t pwm_value);

void pwm_fade(uint8_t pwm_ch, uint8_t fadetime,uint16_t pwm_value, bool wfe);
void pwm_set(uint8_t pwm_ch,uint16_t pwm_value);

void pwm_5ms_tick(void);

void pwm_executeAll(void);
void pwm_execute(uint8_t pwm_ch);

#endif /* PWM_H_ */