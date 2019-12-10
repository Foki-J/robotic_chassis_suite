/******
 *    
 *      Òý½ÅÅäÖÃ
 ******/
 
#include "gpio.h"
//#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_rcc.h"


void gpio_Init(void)
{  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC,ENABLE);
	//ÂÌµÆ
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15|GPIO_Pin_3;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      	
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;      	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//À¶µÆ	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;      	
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	
	//µÆÃð
	LED0 = 1;	
	LED1 = 0;
	LED2 = 0;
	LED3 = 0;
	LED4 = 0;
	
}
