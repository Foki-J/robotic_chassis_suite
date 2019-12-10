#ifndef __CATCH_ARM_H_
#define __CATCH_ARM_H_
#include "sys.h"

extern u16 star_flag;
extern int star1;
extern int star2;
extern int star;
extern int over1_flag;
extern int over2_flag;
extern int new_flag;
void star_init(void);
void TIM2_Init(u16, u16);
void TIM4_Init(u16, u16);
extern int ABS(u16 a,u16 b);
extern u8 phasecw[8]; 
extern u8 phaseccw[8];

void pace_motor1_init(void);
void pace_motor2_init(void);
void SetMotor1(unsigned char InputData);
void SetMotor2(unsigned char InputData);
void CATCHNcircle(int n,int position);
void UPNcircle(int n,int position);
void action (void);
#endif
