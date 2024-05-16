/*
 * SharedFunctions.h
 *
 * Created: 01.08.2017 00:33:17
 *  Author: Christian
 */ 


#ifndef SHAREDFUNCTIONS_H_
#define SHAREDFUNCTIONS_H_

//#define reset_watchdog() asm("WDR")
//#define ADDRESS GPIOR0

uint8_t get_address(void);

uint8_t com_handler(void);

void com_transmit_data(uint8_t instruction_byte, uint8_t *data, uint8_t size, bool is_nAck);

//void callback_5ms_tick(void);
//void callback_com_receive_data(uint8_t instruction_byte, uint8_t *data, uint8_t size);


#endif /* SHAREDFUNCTIONS_H_ */