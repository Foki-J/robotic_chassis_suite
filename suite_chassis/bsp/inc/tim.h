#ifndef _TIM_H
#define _TIM_H

#include "sys.h"
extern uint8_t TIM7_Flag;
extern uint16_t distance;
void TIM14_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void Wave_SRD_Strat(void);
void get_Distance(void);
void tim7_Init(u16 period);
void pwm_TIM3_Init(void);

#endif
