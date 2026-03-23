#ifndef __DISATANCE_H_
#define __DISATANCE_H_


#include "stm32f10x.h"

extern uint64_t Dis_time;

void HC_SR04_Init(void);
int16_t sonar_mm(void);
float sonar(void);



#endif
