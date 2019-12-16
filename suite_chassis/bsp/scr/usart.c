#include "usart.h"
#include "control.h"
#include "gpio.h"
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//Flag_Show!=0  ʹ�ô���1   
	USART1->DR = (u8) ch;      	
	return ch;
}
#endif
///////////////////////////////////////
void usart1_init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	NVIC_InitTypeDef   NVIC_InitStructure;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
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
 
 NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//�ⲿ�ж�0
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//�����ȼ�1
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
 NVIC_Init(&NVIC_InitStructure);//����
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
