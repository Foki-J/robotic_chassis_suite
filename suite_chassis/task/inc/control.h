#ifndef __PID_H
#define __PID_H

#include "sys.h"
#define ABS_(x) ((x>0)?x:(-x))
typedef struct
{
	int16_t   error;
	int16_t   error_Last;
	long      integral;
	float     Kp;
	float     Ki;
	float     Kd;
	float     index;
	int16_t   umax;
	int16_t   umin;
	int16_t   setValue;
	int16_t   actualValue;
	int16_t   output;
} PIDTypeDef;
typedef struct
{
	short vx;
	short vy;
	short w;
	short theta;
	short coordinateSwitch;
	short v1;
	short v2;
	short v3;
} cmd_vel_TypeDef;
extern PIDTypeDef *motorPID;
extern cmd_vel_TypeDef cmd_vel;
void PIDCalculate(PIDTypeDef* PID);
void process(void);
#endif
