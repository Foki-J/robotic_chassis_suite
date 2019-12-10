#include "adc.h"
#include "sys.h"
#include "delay.h" 
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "init.h"


void adc_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC3,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_4;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	ADC_CommonInitStructure.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;                        //��ʹ������ģʽ
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;                //�����Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;            //ȡ��������⣬ʹ��������
	ADC_InitStructure.ADC_NbrOfConversion=1;                         //ת����������Ϊ1
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;            //12λģʽ
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;	
	ADC_Init(ADC1,&ADC_InitStructure);                           //��ʼ��adc
	ADC_Init(ADC3,&ADC_InitStructure); 

	ADC_Cmd(ADC1,ENABLE);
	ADC_Cmd(ADC3,ENABLE);
}
/*
 *  ��õư�Ҷ�ֵ��ͨ������ƽ��ֵ�����˲�
 */
uint16_t grey_Sample[NUM_CHANNEL][NUM_FILTER+1] = {0};
uint16_t grey_Buffer[NUM_CHANNEL] = {0};
uint8_t filter_count = 0;

void get_Adc(void)
{	
//												/***ADC1***/
	//PA5 ͨ��5
	ADC_RegularChannelConfig(ADC1,5,1,ADC_SampleTime_480Cycles);             
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                   
	grey_Sample[0][filter_count] = ADC_GetConversionValue(ADC1);
	
	//PA7 ͨ��7
	ADC_RegularChannelConfig(ADC1,7,1,ADC_SampleTime_480Cycles);             
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[1][filter_count] = ADC_GetConversionValue(ADC1);    
	
  //PB1 ͨ��9
  ADC_RegularChannelConfig(ADC1,9,1,ADC_SampleTime_480Cycles);               
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[2][filter_count] = ADC_GetConversionValue(ADC1); 
	
	//PB0 ͨ��8
  ADC_RegularChannelConfig(ADC1,8,1,ADC_SampleTime_480Cycles);               
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[3][filter_count] = ADC_GetConversionValue(ADC1); 
	
	//PC4 ͨ��14
	ADC_RegularChannelConfig(ADC1,14,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC1);                       
  while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[4][filter_count] = ADC_GetConversionValue(ADC1);
	
	//PA6 ͨ��6
	ADC_RegularChannelConfig(ADC1,6,1,ADC_SampleTime_480Cycles);               
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[5][filter_count] = ADC_GetConversionValue(ADC1); 
	
	//PA4 ͨ��4
	ADC_RegularChannelConfig(ADC1,4,1,ADC_SampleTime_480Cycles);               
	ADC_SoftwareStartConv(ADC1);                       
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[6][filter_count] = ADC_GetConversionValue(ADC1); 
	
	
												/***ADC3***/

	//PF3 ͨ��9
	ADC_RegularChannelConfig(ADC3,9,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                     
	grey_Sample[7][filter_count] = ADC_GetConversionValue(ADC3); 
	
	//PF5 ͨ��15
	ADC_RegularChannelConfig(ADC3,15,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                     
	grey_Sample[8][filter_count] = ADC_GetConversionValue(ADC3); 
  
	//PF7 ͨ��5
	ADC_RegularChannelConfig(ADC3,5,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC3);                       
  while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[9][filter_count] = ADC_GetConversionValue(ADC3);
	
	//PC1 ͨ��11
	ADC_RegularChannelConfig(ADC3,11,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                     
	grey_Sample[10][filter_count] = ADC_GetConversionValue(ADC3); 
	
	//PC3 ͨ��13
	ADC_RegularChannelConfig(ADC3,13,1,ADC_SampleTime_480Cycles);              
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                     
	grey_Sample[11][filter_count] = ADC_GetConversionValue(ADC3); 
	
	//PF4 ͨ��14
	ADC_RegularChannelConfig(ADC3,14,1,ADC_SampleTime_480Cycles);             
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                   
	grey_Sample[12][filter_count] = ADC_GetConversionValue(ADC3); 
	
	//PF6 ͨ��4
	ADC_RegularChannelConfig(ADC3,4,1,ADC_SampleTime_480Cycles);             
	ADC_SoftwareStartConv(ADC3);                       
	while(!ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)/*�ȴ�ת��*/);                    
	grey_Sample[13][filter_count] = ADC_GetConversionValue(ADC3);    	 											
	
  filter_count++;
	
	if(filter_count == NUM_FILTER)
		filter_count = 0;                      //���ζ��и���
	
	for(int i = 0; i<NUM_CHANNEL; i++)       //���ֵ
	{
		grey_Sample[i][NUM_FILTER] = 0;
		for(int j = 0; j<NUM_FILTER; j++)
		{
			grey_Sample[i][NUM_FILTER] += grey_Sample[i][j];
		}
		grey_Buffer[i] = grey_Sample[i][NUM_FILTER]/NUM_FILTER;
	}
	
}
