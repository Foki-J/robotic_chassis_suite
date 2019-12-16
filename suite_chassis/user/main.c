#include "sys.h"
#include "control.h"
#include "DataScope_DP.h"
#include "exti.h"
#include "gpio.h"
#include "software_Debug.h"
#include "init.h"
#include "catch_arm.h"
#include "DataScope_DP.h"
#include "mpu6050.h"
#include "tim.h"
int timer = 0;
int main(void)
{
	init();
	while (1)
	{
		if(!TIM7_Flag)//周期10ms
			continue;
		timer++;
		TIM7_Flag=0;
		process();
		if(PBin(11)==0)
			modeSwitch=0;
		switch(modeSwitch)
		{
			case 0:
				LED1=1;
				LED2=0;
				LED3=0;
				LED4=0;
			break;
			case 1:
				LED1=0;
				LED2=1;
				LED3=0;
				LED4=0;
			break;
			case 2:
				LED1=0;
				LED2=0;
				LED3=1;
				LED4=0;
			break;
			case 3:
				LED1=0;
				LED2=0;
				LED3=0;
				LED4=1;
			break;
		}
		if((timer%50)==0)//周期500ms
			LED0=!LED0;
	}
}

	
