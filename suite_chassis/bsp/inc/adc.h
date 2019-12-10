#ifndef _adc_H
#define _adc_H
#include "sys.h"

#define NUM_FILTER 10
#define NUM_CHANNEL 14

extern uint16_t grey_Sample[NUM_CHANNEL][NUM_FILTER+1];
extern uint16_t grey_Buffer[NUM_CHANNEL];

void adc_Init(void); 				
void get_Adc(void); 				//获得数组

#endif
