/******
 *    DMA相关
 *      DMA2的初始化配置
 *      DMA2接收中断函数
 *      获取DMA2中断标识
 *      清空DMA2中断标识
 *      对遥控器发来的数据处理，变为Vy,Vx,Theta_setValue的赋值
 ******/
 
#include "dma.h"
#include "init.h"
#include "sys.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include <math.h>
#include "stm32f4xx_gpio.h"

#define pi  3.1415926535
//遥控器数据缓存长度
#define dma_Buffer_Length 18

//dma存储地址
uint8_t DBUSBuffer[dma_Buffer_Length];

/**
 *      DMA2 channel4的配置
 */
 
void dma2_Init(void)
{
	DMA_InitTypeDef dma;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	NVIC_InitTypeDef nvic;
	
  DMA_DeInit(DMA2_Stream2);
	dma.DMA_Channel= DMA_Channel_4; //DMA请求映射 数据流二通道四
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);  //外设寄存器 地址
	dma.DMA_Memory0BaseAddr = (uint32_t)DBUSBuffer; //储存器地址
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory; //传输方向
	dma.DMA_BufferSize = 18;  //传输数据字节
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设寄存器加一不使能
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable; // 储存器地址加一
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //DMA接受的的数据类型
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular; //DMA循环模式
	dma.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道优先级
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable; //FIFO缓冲模式关闭
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_Single; 
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //一次突发
	DMA_Init(DMA2_Stream2,&dma);
	
	DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE); //DMA 2 数据流2的接受完成中断开启
	DMA_Cmd(DMA2_Stream2,ENABLE); //DMA通道使能
	
	nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
}

uint8_t DMA2_Flag = 0;                 //DMA2中断标识
/**
 *       DMA2中断函数
 *       开启中断标志位 DMA2_FLAG
 */
void DMA2_Stream2_IRQHandler(void)
{
 if(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2))
 {
   DMA2_Flag = 1;	
	 DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
	 DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF2);
 }
}

/**
 *      返回中断标识
 */
uint8_t get_DMA2_Flag_Status(void)
{
	return DMA2_Flag;
}

/**
 *      清空中断标识
 */
void clear_DMA2_Flag(void)
{
	DMA2_Flag = 0;
}

/**
 *      dma_Buffer数据解码
 *      解码后数据储存于 remote_Data_Struct结构体
 *      用解码后的数据给move_Speed_Struct结构体赋值
 */

void dma_Data_Decoding(void)
{
  control_data->ch1 = -((DBUSBuffer[0] |(DBUSBuffer[1]<<8)) & 0x07FF) + 1024;
	control_data->ch2 = -((DBUSBuffer[1]>>3 | DBUSBuffer[2]<<5 ) & 0x07FF) + 1024;
	control_data->ch3 = -((DBUSBuffer[2]>>6 | DBUSBuffer[3]<<2 | DBUSBuffer[4]<<10) & 0x07FF)+1024;
	control_data->ch4 = -((DBUSBuffer[4]>>1 | DBUSBuffer[5]<<7) & 0x07FF)+1024;
	
	control_data->switch_Left = ( (DBUSBuffer[5] >> 4)& 0x000C ) >> 2;
	control_data->switch_Right =  (DBUSBuffer[5] >> 4)& 0x0003;
	
	control_data->ch3 = (control_data->ch3*get_ABS(control_data->ch3)) / 500;
	control_data->ch4 = (control_data->ch4*get_ABS(control_data->ch4)) / 500;
	
	if(control_data->ch2>640)
	{
		platMotor_data->setValue = 300;
	  GPIO_ResetBits(GPIOA,GPIO_Pin_1);		
	  GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
	else if(control_data->ch1>640)
	{
		platMotor_data->setValue = 15500;
		GPIO_SetBits(GPIOA,GPIO_Pin_1);	
	  GPIO_ResetBits(GPIOA,GPIO_Pin_0);	
	}
	else if(control_data->ch2<-640)
	{
		platMotor_data->setValue = 31200;
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);		
	  GPIO_SetBits(GPIOA,GPIO_Pin_0);
	}
	else if(control_data->ch1<-640)
	{
	  platMotor_data->setValue = -19000;
		GPIO_SetBits(GPIOA,GPIO_Pin_1);		
	  GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	}
	
	/**
	 *     ch3,ch4 [-660, 660]
	 *     ch2 {-1,0,1} 
	 *     
	 *     ch3,4微调
	 *     ch2 定位
	 */
}
