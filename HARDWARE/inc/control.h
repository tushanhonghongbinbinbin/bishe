#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"

#define Water 0
#define Fan 1
#define Hot 2

#define ON 1
#define OFF 2

#define SpeedIn GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)	


#define Water_ON			GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define Water_OFF			GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define Fan_ON			GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define Fan_OFF			GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define Hot_ON			GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define Hot_OFF			GPIO_SetBits(GPIOB, GPIO_Pin_14)

void Device_Task(uint8_t _dev, uint8_t _mode);

void DeviceControl_Init(void);

#endif


