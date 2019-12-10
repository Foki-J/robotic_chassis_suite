#ifndef _SOFTDEBUG_H
#define _SOFTDEBUG_H	 

#include "sys.h" 

void usart_sendData(uint8_t msg);

void send_u8_Data(uint8_t* data);

void send_2_Data(int16_t data1);

void send_1_Data(int16_t data);

void send_3_Data(int16_t data1,int16_t data2,int16_t data3);

void send_4_Data(int16_t data1,int16_t data2,int16_t data3,int16_t data4);

void send_Grey_Data(void);

void send_PID_Data(void);

void send_fire(int16_t data1,int16_t data2,int16_t data3);
	
#endif
