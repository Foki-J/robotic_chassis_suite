/******
 *    ���ô���2����λ������֡
 ******/
#include "software_Debug.h"
#include "usart.h"	
#include "catch_arm.h"
/**
*    �ɴ���2��������
*    ������8λ����
*/
void usart_sendData(uint8_t msg)
{		
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
	                     //while �Ǵ������
	USART_SendData(USART1,msg);				
}	

uint8_t length = 4;	   //��������ݸ���

/**
*    �ɴ���2����֡
*    ����������Ϊlength��8λ����
*/	
void send_u8_Data(uint8_t* data)
{
	uint16_t sum = 0x88 + 0xA1 + length;
	                     //sum��֡��У��λ�������
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
*    ����λ�����Ͱ���3�����ݵ�֡
*    ������3��16λ����
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
*    ����λ�����Ͱ���3�����ݵ�֡
*    ������3��16λ����
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
*    ����λ�����Ͱ���һ�����ݵ�֡
*    ������һ��16λ����
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

