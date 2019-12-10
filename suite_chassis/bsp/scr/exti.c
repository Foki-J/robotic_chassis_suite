#include "exti.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
void exti_init(void)
{  
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
 	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOB时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //KEY对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//PB15 

	/* 配置EXTI_Line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	//按键
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;      	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3|EXTI_Line7|EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}
char delay_flag,delay_20;
char modeSwitch = 0;
void EXTI15_10_IRQHandler(void) 
{
	 if(INT==0)		
	{  
   		EXTI->PR=1<<15;                                                      //清除中断标志位 
			EXTI_ClearITPendingBit(EXTI_Line15);
		  if(delay_flag==1)
			 {
				 if(++delay_20==4)	 delay_20=0,delay_flag=0;                     //给主函数提供50ms的精准延时
			 }
			Read_DMP();                      //===读取加速度、角速度、倾角
	}
}

void EXTI3_IRQHandler(void) 
{    
	 if(EXTI_GetITStatus(EXTI_Line3))
	{
    modeSwitch = 3;
	  EXTI_ClearITPendingBit(EXTI_Line3);
	}    		  
}

void EXTI9_5_IRQHandler(void) 
{    
	 if(EXTI_GetITStatus(EXTI_Line7))
	{
    modeSwitch = 2;
	  EXTI_ClearITPendingBit(EXTI_Line7);
	} 
	 if(EXTI_GetITStatus(EXTI_Line5))
	{
    modeSwitch = 1;
	  EXTI_ClearITPendingBit(EXTI_Line5);
	}
}


