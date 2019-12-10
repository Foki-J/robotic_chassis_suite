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
void process(void)
{
	
	for(int i=0;i<4;i++)
	{
		motorPID[i].actualValue = motorSpeed[i];
	}
	cosTheta = 1;
	sinTheta = 0;
//	cmd_vel.vy = 1000;
//	cmd_vel.vx = 1000;
	//�˶�ѧ����
	motorPID[0].setValue =  (int16_t)(( cosTheta + sinTheta)*cmd_vel.vx + (-cosTheta + sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[1].setValue =  (int16_t)(( cosTheta - sinTheta)*cmd_vel.vx + ( cosTheta + sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[2].setValue =  (int16_t)((-cosTheta - sinTheta)*cmd_vel.vx + ( cosTheta - sinTheta)*cmd_vel.vy - cmd_vel.w);
	motorPID[3].setValue =  (int16_t)((-cosTheta + sinTheta)*cmd_vel.vx + (-cosTheta - sinTheta)*cmd_vel.vy - cmd_vel.w);
  
	PIDCalculate(motorPID);
	PIDCalculate(motorPID+1);
	PIDCalculate(motorPID+2);
	PIDCalculate(motorPID+3);
	
	send_CAN1_Message(motorPID);
	send_3_Data(cmd_vel.vx,cmd_vel.vy,cmd_vel.w);
}

/**
 *    PID����
 *    ������PID�ṹ��ָ��
 */
void PIDCalculate(PIDTypeDef* PID)
{
	PID->error = PID->setValue - PID->actualValue;
	
	//������	
	if (PID->output >= PID->umax)
	{
		PID->integral += ((PID->error > 0) ? 0 : PID->error);   //�����ֵ�������ޣ���ֻ����ƫ��
	}	
  else if (PID->output <= PID->umin)
	{
		PID->integral += ((PID->error < 0) ? 0 : PID->error);   //�����ֵ�������ޣ���ֻ����ƫ��
	}
	else
	{
		PID->integral += PID->error;
	}

	PID->output  =   PID->Kp * PID->error                       //��������
	               + PID->Ki * PID->integral       //���ֻ���
	               + PID->Kd * (PID->error - PID->error_Last);  //΢�ֻ���
		
	PID->error_Last = PID->error;//�������
}










	

