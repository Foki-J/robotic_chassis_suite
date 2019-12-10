#ifndef GPIO_H
#define GPIO_H

#include "sys.h"

#define LED0 PBout(13)
#define LED1 PCout(13)
#define LED2 PCout(15)
#define LED3 PCout(3)
#define LED4 PAout(1)

void gpio_Init(void);

#endif
