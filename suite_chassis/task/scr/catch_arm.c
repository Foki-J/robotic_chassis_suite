#include "sys.h"
#include "catch_arm.h"
#include "gpio.h"
u16 star_flag=0;
int over1_flag=0;
int over2_flag=0;
int new_flag=0;
int star1=0;
int star2=0;
int star=0;
/*************
开始信号和陀螺仪数据
**************/
int ABS(uint16_t X,uint16_t Y)		
{
	return (((X-Y)>0)?((X-Y)):(Y-X)); 
}
void star_init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	//初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);            
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//设置为中断请求，非事件请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//设置中断触发方式为下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//外部中断使能
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5))	//PF0又被触发
	{ 
		star_flag=1;
		EXTI_ClearITPendingBit(EXTI_Line5); //*
	}  
						
}
/*************
定时器TIM2\4
**************/
void TIM2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使定时器时钟
															//初始化定时器，配置ARR,PSC
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;

	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启定时器中断
	TIM_Cmd(TIM2,ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;//tim2中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;//优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x00;//子优先级为1
	
	NVIC_Init(&NVIC_InitStruct);//配置NVIC
	//使能定时器
}

void TIM2_IRQHandler(void)	//编写中断服务函数	
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		star1=1;//
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使定时器时钟
															//初始化定时器，配置ARR,PSC
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_Period=arr;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//开启定时器中断
	TIM_Cmd(TIM4,ENABLE);
	NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;//tim2中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;//优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;//子优先级为1
	
	NVIC_Init(&NVIC_InitStruct);//配置NVIC
	//使能定时器
}

void TIM4_IRQHandler(void)	//编写中断服务函数	
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		star2=1;//
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
	
}
/**************
步进电机初始化
**************/
u8 phasecw[8] ={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};		//clockwise
u8 phaseccw[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};		//anticlockwise
int i,j,k,n0,position0=0;
int l,m,p,n1,position1=0;
void pace_motor1_init(void)                                   //init step_motor 1
{	
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
			
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);     				//PAin 0,1,2,3
}
void pace_motor2_init(void)                 				//init step_motor 2
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
			
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);     				//PCin 6,7,8,9
}
void SetMotor1(unsigned char InputData)                     //set step motor2
{
    if(InputData & 0x01)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_0);
    }
    if(InputData & 0x02)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_1);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_1);

    }
    if(InputData & 0x04)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_2);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_2);
    }
    if(InputData & 0x08)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_3);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_3);
    }
}
void SetMotor2(unsigned char InputData)
{
    if(InputData & 0x01)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_6);
    }
    if(InputData & 0x02)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_7);

    }
    if(InputData & 0x04)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_8);
    }
    else
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_8);
    }
    if(InputData & 0x08)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_9);
    }
    else
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_9);
    }
}
void CATCHNcircle(int n,int position)
{
	if (star1==1)
	{

		if (j<n)
		{
					if(i<17*3)
					{
							if (k<8)
							{
								if(1 == position)
								{
									SetMotor1(phasecw[k]);
								}
								else
								{
									SetMotor1(phaseccw[k]);
								}
								k++;
							}
						  if(k==8)
							{
							i++;
							k=0;						
							}
					}
					if(i==51)
					{
					j++;	
					i=0;
					}
		}
		if (j==n)
		{
			over1_flag=1;
		}
	star1=0;
	}
}

void UPNcircle(int n,int position)
{
	if (star2==1)
	{
		if (new_flag==1)
		{
			l=0;
			new_flag=0;
		}
		if (l<n)
		{
			if(m<17*3)
			{
					if (p<8)
					{
						if(1 == position)
						{
							SetMotor2(phasecw[p]);
						}
						else
						{
							SetMotor2(phaseccw[p]);
						}
						p++;
				}
								if(p==8)
								{
								m++;
								p=0;						
								}
			}
					if(m==51)
					{
					l++;	
					m=0;
					}
		}
		if (l==n)
		{
			over2_flag=1;
		}
	star2=0;
	}
}
void action (void)
{
	while(star_flag==0)
	{
	};
	TIM_Cmd(TIM2,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
		
}
