#ifndef EXTI_H
#define EXTI_H
#include "sys.h"
#define INT PBin(15)   //���ӵ�MPU6050���ж�����
extern char delay_flag,delay_20;
extern char modeSwitch;
void exti_init(void);
#endif
