#ifndef EXTI_H
#define EXTI_H
#include "sys.h"
#define INT PBin(15)   //连接到MPU6050的中断引脚
extern char delay_flag,delay_20;
extern char modeSwitch;
void exti_init(void);
#endif
