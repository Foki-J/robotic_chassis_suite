/******
 *    利用串口2向上位机发送帧
 ******/
#include "software_Debug.h"
#include "usart.h"	
#include "catch_arm.h"
/**
*    由串口2传输数据
*    参数：8位数据
*/
void usart_sendData(uint8_t msg)
{		
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
	                     //while 非传输完成
	USART_SendData(USART1,msg);				
}	

uint8_t length = 4;	   //传输的数据个数

/**
*    由串口2发送帧
*    参数：长度为length的8位数组
*/	
void send_u8_Data(uint8_t* data)
{
	uint16_t sum = 0x88 + 0xA1 + length;
	                     //sum是帧的校验位，最后发送
	usart_sendData(0x88);	
	usart_sendData(0xA1);
	usart_sendData(length);
	
	for (uint8_t i = 0;; i++)
	{
		if(i == length)
			break;
	  usart_sendData(data[i]);
		sum += data[i];
	}
	usart_sendData(sum);		
}

/**
*    向上位机发送包含3个数据的帧
*    参数：3个16位数据
*/
void send_3_Data(int16_t data1,int16_t data2,int16_t data3)
{
	length = 6;
	uint8_t tbuf[6] = {0}; 
	
	tbuf[0] = data1 >> 8;
	tbuf[1] = data1;
	
	tbuf[2] = data2 >> 8;
	tbuf[3] = data2;

	tbuf[4] = data3 >> 8;
	tbuf[5] = data3;
	
	send_u8_Data(tbuf);
}
/**
*    向上位机发送包含3个数据的帧
*    参数：3个16位数据
*/
void send_4_Data(int16_t data1,int16_t data2,int16_t data3, int16_t data4)
{
	length = 8;
	uint8_t tbuf[8] = {0}; 
	
	tbuf[0] = data1 >> 8;
	tbuf[1] = data1;
	
	tbuf[2] = data2 >> 8;
	tbuf[3] = data2;
	
	tbuf[4] = data3 >> 8;
	tbuf[5] = data3;

	tbuf[6] = data4 >> 8;
	tbuf[7] = data4;
	
	send_u8_Data(tbuf);
}



/**
*    向上位机发送包含一个数据的帧
*    参数：一个16位数据
*/
void send_1_Data(int16_t data)
{
	length = 2;
	uint8_t tbuf[2] = {0}; 
	
	tbuf[0] = data >> 8;
	tbuf[1] = data;

	send_u8_Data(tbuf);
}

void send_fire(int16_t data1,int16_t data2,int16_t data3)
{
	length = 6;
	int8_t tbuf[6] = {0}; 

	usart_sendData(0x33);
	usart_sendData(0x66);
	
	tbuf[0] = data1 >> 8;
	tbuf[1] = data1;
	
	tbuf[2] = data2 >> 8;
	tbuf[3] = data2;

	tbuf[4] = data3 >> 8;
	tbuf[5] = data3;
	
	for (uint8_t i = 0;; i++)
	{
		if(i == length)
			break;
	    usart_sendData(tbuf[i]);
	}
	usart_sendData(0x99);
}

