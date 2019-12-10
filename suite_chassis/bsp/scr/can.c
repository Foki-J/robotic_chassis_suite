/******
 *    CAN���
 *      CAN1�ĳ�ʼ������
 *      CAN1�����жϺ���
 * 		  ��ȡCAN1���յ�ת��ֵ
 *		  ����CAN��Ϣ
 ******/
 
#include "can.h"
#include "sys.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_can.h"
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "tim.h"
#include "init.h"
#include "control.h"
/**
*       ��ʼ��can1
*       ������������(Kbps)
*/
void can1_Init()
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��PORTBʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  //ʹ��CAN1ʱ��	
	  
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
    GPIO_Init(GPIOB, &GPIO_InitStructure);                //��ʼ��
	
	  //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); //GPIOB8����ΪCAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOB9����ΪCAN1
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	                  //��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	                  //����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;                   //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	                  //��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	                  //���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	                  //���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	          //��ͨģʽ 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	              //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS2_6tq;                //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS1_7tq;                //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler = 3;    //������ = 42Mhz / (6 + 7 + 1) / ��Ƶϵ�� 	
  	CAN_Init(CAN1, &CAN_InitStructure);                   // ��ʼ��CAN1 
    
		//���ù�����
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	          //������0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;      //32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;  //32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;  //���������0
  	CAN_FilterInit(&CAN_FilterInitStructure);             //�˲�����ʼ��
	  
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);                //FIFO0��Ϣ�Һ��ж�����.		    
    
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);		
}   

/**
*    can1�����ж�
*    ����֡��һ��λΪת��ֵ
*            ����λΪת��ֵ
*            ����λΪת�ص���ֵ
*            ����λΪ����¶�
*/
short motorSpeed[4] = {0};
void CAN1_RX0_IRQHandler(void)
{ 
  CanRxMsg rx_Message;
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
    CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    CAN_Receive(CAN1, 0, &rx_Message);   
		switch(rx_Message.StdId)
		{
			case 0x201:
				motorSpeed[0] = ((int16_t)rx_Message.Data[2]<<8|rx_Message.Data[3]);	
			break;
			case 0x202:
				motorSpeed[1] = ((int16_t)rx_Message.Data[2]<<8|rx_Message.Data[3]);	
			break;
			case 0x203:
				motorSpeed[2] = ((int16_t)rx_Message.Data[2]<<8|rx_Message.Data[3]);	
			break;
			case 0x204:
				motorSpeed[3] = ((int16_t)rx_Message.Data[2]<<8|rx_Message.Data[3]);	
			break;
		}
	}
}

/**
*    can����һ������(�̶���ʽ:IDΪ0x200,��׼֡,����֡)	
*    len:���ݳ���(���Ϊ8)				     
*    msg:����ָ��,���Ϊ8���ֽ�.
*/
void send_CAN1_Message(PIDTypeDef* motorPID)
{
	uint8_t mbox;
  uint16_t i=0;
	
  CanTxMsg TxMessage;
  TxMessage.StdId = 0x200;	 // ��׼��ʶ��Ϊ0x200(11λ��
  TxMessage.ExtId = 0;	     // ������չ��ʾ����29λ��
  TxMessage.IDE = 0;		     // ��ʹ����չ��ʶ��
  TxMessage.RTR = 0;		     // ��Ϣ����Ϊ����֡
  TxMessage.DLC = 8;			   // ����len֡��Ϣ
	
  TxMessage.Data[0] = motorPID[0].output>> 8;			
  TxMessage.Data[1] = motorPID[0].output;
  TxMessage.Data[2] = motorPID[1].output>> 8;
  TxMessage.Data[3] = motorPID[1].output;
  TxMessage.Data[4] = motorPID[2].output >> 8;		
  TxMessage.Data[5] = motorPID[2].output;
	TxMessage.Data[6] = motorPID[3].output >> 8;	
	TxMessage.Data[7] = motorPID[3].output;
	
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	
  i=0;
	
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0xfff))
	 {
	   i++;	//�ȴ����ͽ���
	 }	 
}


