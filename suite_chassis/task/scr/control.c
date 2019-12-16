/******
 *    
 ******/
 
#include "control.h"
#include "software_Debug.h"
#include "sys.h"
#include "delay.h"
#include "gpio.h"
#include "usart.h"
#include "exti.h"
#include "DataScope_DP.h"
#include "can.h"
#include "tim.h"

PIDTypeDef motorPID_Struct[4];
PIDTypeDef *motorPID = motorPID_Struct;
cmd_vel_TypeDef cmd_vel;
char coordinateSwitch = 1;
char cosTheta = 0;
char sinTheta = 0;
#define FILTER_LENGTH 30
int VelFilterBuffer[3*FILTER_LENGTH+4] = {0};//0:counter,1-10:vxBuffer,11:vxBufferSum,12-21:vy,22:vySum,23-32:w,33:wSum
void process(void)
{	
	for(int i=0;i<4;i++)
	{
		motorPID[i].actualValue = motorSpeed[i];
	}
	//滤波
	VelFilterBuffer[VelFilterBuffer[0]+1]=cmd_vel.vx;
	VelFilterBuffer[VelFilterBuffer[0]+2+FILTER_LENGTH]=cmd_vel.vy;
	VelFilterBuffer[VelFilterBuffer[0]+3+FILTER_LENGTH*2]=cmd_vel.w;
	VelFilterBuffer[FILTER_LENGTH+1]=0;
	VelFilterBuffer[FILTER_LENGTH*2+2]=0;
	VelFilterBuffer[FILTER_LENGTH*3+3]=0;
	for(char i=0;i<FILTER_LENGTH;i++)
	{
		VelFilterBuffer[FILTER_LENGTH+1] += VelFilterBuffer[i+1];
		VelFilterBuffer[FILTER_LENGTH*2+2] += VelFilterBuffer[i+FILTER_LENGTH+2];
		VelFilterBuffer[FILTER_LENGTH*3+3] += VelFilterBuffer[i+FILTER_LENGTH*2+3];
	}
	cmd_vel.v1 = VelFilterBuffer[FILTER_LENGTH+1]/FILTER_LENGTH;
	cmd_vel.v2 = VelFilterBuffer[FILTER_LENGTH*2+2]/FILTER_LENGTH;
	cmd_vel.v3 = VelFilterBuffer[FILTER_LENGTH*3+3]/FILTER_LENGTH;
	VelFilterBuffer[0] = (VelFilterBuffer[0]==FILTER_LENGTH-1)?0:VelFilterBuffer[0]+1;

	cosTheta = 1;
	sinTheta = 0;
	//运动学解算
	motorPID[0].setValue =  (int16_t)(( cosTheta + sinTheta)*cmd_vel.vx + (-cosTheta + sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[1].setValue =  (int16_t)(( cosTheta - sinTheta)*cmd_vel.vx + ( cosTheta + sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[2].setValue =  (int16_t)((-cosTheta - sinTheta)*cmd_vel.vx + ( cosTheta - sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[3].setValue =  (int16_t)((-cosTheta + sinTheta)*cmd_vel.vx + (-cosTheta - sinTheta)*cmd_vel.vy - cmd_vel.w);
  
	PIDCalculate(motorPID);
	PIDCalculate(motorPID+1);
	PIDCalculate(motorPID+2);
	PIDCalculate(motorPID+3);
	
	send_CAN1_Message(motorPID);
	send_4_Data(cmd_vel.vx,cmd_vel.v1,cmd_vel.vy,cmd_vel.v2);
}

/**
 *    PID计算
 *    参数：PID结构体指针
 */
void PIDCalculate(PIDTypeDef* PID)
{
	PID->error = PID->setValue - PID->actualValue;
	
	//抗饱和	
	if (PID->output >= PID->umax)
	{
		PID->integral += ((PID->error > 0) ? 0 : PID->error);   //当输出值超出上限，则只积负偏差
	}	
  else if (PID->output <= PID->umin)
	{
		PID->integral += ((PID->error < 0) ? 0 : PID->error);   //当输出值低于下限，则只积正偏差
	}
	else
	{
		PID->integral += PID->error;
	}

	PID->output  =   PID->Kp * PID->error                       //比例环节
	               + PID->Ki * PID->integral       //积分环节
	               + PID->Kd * (PID->error - PID->error_Last);  //微分环节
		
	PID->error_Last = PID->error;//储存误差
}










	

