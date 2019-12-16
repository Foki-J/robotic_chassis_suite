#include "usart.h"
#include "control.h"
#include "gpio.h"
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//Flag_Show!=0  使用串口1   
	USART1->DR = (u8) ch;      	
	return ch;
}
#endif
///////////////////////////////////////
void usart1_init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
}

void uart5_init(u32 bound)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 USART_InitTypeDef USART_InitStructure;
 NVIC_InitTypeDef   NVIC_InitStructure;

 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
 //TX
 GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
 //RX
 GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); 
 
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
 GPIO_Init(GPIOC,&GPIO_InitStructure); 
 
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
 GPIO_Init(GPIOD,&GPIO_InitStructure);

 USART_InitStructure.USART_BaudRate = bound;
 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 USART_InitStructure.USART_StopBits = USART_StopBits_1;
 USART_InitStructure.USART_Parity = USART_Parity_No;
 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
 USART_Init(UART5, &USART_InitStructure); 
 
 NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//外部中断0
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级1
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
 NVIC_Init(&NVIC_InitStructure);//配置
 USART_Cmd(UART5, ENABLE);
}

void USART1_IRQHandler(void)
{
  if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)  
	{
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
		static unsigned char ch;
		static char count;
		static short buffValue[10];
		ch=USART_ReceiveData(USART1);
    if ((count==0)||(count==1))  
		{
			if (ch == 0xFF)
				count++;
			else count=0;
		}
		else if ((count>1)&&(count<12))
		{
			buffValue[count-2]=ch;
			count++;
		}
		else if (count == 12)
		{
			count=0;
			if (ch == 0xFE)
			{	
				cmd_vel.vx = (short)buffValue[0]<<8|buffValue[1];
				cmd_vel.vy = (short)(buffValue[2]<<8)|buffValue[3];
				cmd_vel.w = (short)(buffValue[4]<<8)|buffValue[5];
				cmd_vel.theta = (short)(buffValue[6]<<8)|buffValue[7];
				cmd_vel.coordinateSwitch = (short)(buffValue[8]<<8)|buffValue[9];
				LED0=!LED0;
			}
		}
	}
}
