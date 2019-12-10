/******
 *    DMA���
 *      DMA2�ĳ�ʼ������
 *      DMA2�����жϺ���
 *      ��ȡDMA2�жϱ�ʶ
 *      ���DMA2�жϱ�ʶ
 *      ��ң�������������ݴ�����ΪVy,Vx,Theta_setValue�ĸ�ֵ
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
//ң�������ݻ��泤��
#define dma_Buffer_Length 18

//dma�洢��ַ
uint8_t DBUSBuffer[dma_Buffer_Length];

/**
 *      DMA2 channel4������
 */
 
void dma2_Init(void)
{
	DMA_InitTypeDef dma;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	NVIC_InitTypeDef nvic;
	
  DMA_DeInit(DMA2_Stream2);
	dma.DMA_Channel= DMA_Channel_4; //DMA����ӳ�� ��������ͨ����
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);  //����Ĵ��� ��ַ
	dma.DMA_Memory0BaseAddr = (uint32_t)DBUSBuffer; //��������ַ
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory; //���䷽��
	dma.DMA_BufferSize = 18;  //���������ֽ�
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //����Ĵ�����һ��ʹ��
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable; // ��������ַ��һ
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //DMA���ܵĵ���������
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular; //DMAѭ��ģʽ
	dma.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�����ȼ�
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable; //FIFO����ģʽ�ر�
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_Single; 
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //һ��ͻ��
	DMA_Init(DMA2_Stream2,&dma);
	
	DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE); //DMA 2 ������2�Ľ�������жϿ���
	DMA_Cmd(DMA2_Stream2,ENABLE); //DMAͨ��ʹ��
	
	nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 3;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
}

uint8_t DMA2_Flag = 0;                 //DMA2�жϱ�ʶ
/**
 *       DMA2�жϺ���
 *       �����жϱ�־λ DMA2_FLAG
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
 *      �����жϱ�ʶ
 */
uint8_t get_DMA2_Flag_Status(void)
{
	return DMA2_Flag;
}

/**
 *      ����жϱ�ʶ
 */
void clear_DMA2_Flag(void)
{
	DMA2_Flag = 0;
}

/**
 *      dma_Buffer���ݽ���
 *      ��������ݴ����� remote_Data_Struct�ṹ��
 *      �ý��������ݸ�move_Speed_Struct�ṹ�帳ֵ
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
	 *     ch3,4΢��
	 *     ch2 ��λ
	 */
}
