#include "init.h"
#include "sys.h"
#include "control.h"
#include "software_Debug.h"
#include "sys.h"
#include "delay.h"
#include "gpio.h"
#include "usart.h"
#include "exti.h"
#include "DataScope_DP.h"
#include "can.h"
#include "tim.h"
#include "catch_arm.h"
/**
 *	硬件和参数初始化
 */
void init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	gpio_Init();
	
	pwm_TIM3_Init();
	tim7_Init(10);
	usart1_init(115200);
	//uart5_init(9600);
	delay_ms(20);
	IIC_Init();
	MPU6050_initialize();           //=====MPU6050初始化	
	DMP_Init();                     //=====初始化DMP 
	exti_init();
	delay_ms(100);
	can1_Init();
	delay_ms(50);
	for(int i=0;i<4;i++)
	{
		motorPID[i].Kp = 2;
		motorPID[i].Ki = 0.01;
		motorPID[i].Kd = 0.1;
	}
}
