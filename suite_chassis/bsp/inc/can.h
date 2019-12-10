#ifndef __CAN_H
#define __CAN_H	 

#include "sys.h" 		
#include "control.h"
extern short motorSpeed[4];

void can1_Init(void);
void send_CAN1_Message(PIDTypeDef* input);
#endif

















