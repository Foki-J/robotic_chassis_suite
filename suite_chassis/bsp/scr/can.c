/******
 *    CAN相关
 *      CAN1的初始化配置
 *      CAN1接收中断函数
 * 		  获取CAN1接收的转速值
 *		  发送CAN消息
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
*       初始化can1
*       参数：波特率(Kbps)
*/
void can1_Init()
{
  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
   	NVIC_InitTypeDef  NVIC_InitStructure;

    //使能相关时钟
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能PORTB时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);  //使能CAN1时钟	
	  
    //初始化GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                //初始化
	
	  //引脚复用映射配置
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); //GPIOB8复用为CAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOB9复用为CAN1
	  
  	//CAN单元设置
   	CAN_InitStructure.CAN_TTCM=DISABLE;	                  //非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	                  //软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;                   //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	                  //禁止报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	                  //报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	                  //优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	          //普通模式 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	              //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS2_6tq;                //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS1_7tq;                //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler = 3;    //波特率 = 42Mhz / (6 + 7 + 1) / 分频系数 	
  	CAN_Init(CAN1, &CAN_InitStructure);                   // 初始化CAN1 
    
		//配置过滤器
 	  CAN_FilterInitStructure.CAN_FilterNumber=0;	          //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;      //32位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;  //32位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;  //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);             //滤波器初始化
	  
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);                //FIFO0消息挂号中断允许.		    
    
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);		
}   

/**
*    can1接收中断
*    接收帧的一二位为转角值
*            二三位为转速值
*            四五位为转矩电流值
*            第六位为电机温度
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
*    can发送一组数据(固定格式:ID为0x200,标准帧,数据帧)	
*    len:数据长度(最大为8)				     
*    msg:数据指针,最大为8个字节.
*/
void send_CAN1_Message(PIDTypeDef* motorPID)
{
	uint8_t mbox;
  uint16_t i=0;
	
  CanTxMsg TxMessage;
  TxMessage.StdId = 0x200;	 // 标准标识符为0x200(11位）
  TxMessage.ExtId = 0;	     // 设置扩展标示符（29位）
  TxMessage.IDE = 0;		     // 不使用扩展标识符
  TxMessage.RTR = 0;		     // 消息类型为数据帧
  TxMessage.DLC = 8;			   // 发送len帧信息
	
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
	   i++;	//等待发送结束
	 }	 
}


