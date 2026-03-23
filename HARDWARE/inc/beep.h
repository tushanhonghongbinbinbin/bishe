#ifndef __BEEP_H
#define __BEEP_H
#include "sys.h"


#define BEEP_GPIO_RCC  RCC_APB2Periph_GPIOA
#define BEEP_GPIO_PORT GPIOA
#define BEEP_GPIO_PIN  GPIO_Pin_1


#define BEEP_ON GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
#define BEEP_OFF GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)



void VoiceSendData(u8 addr );
void Voice_Init(void);
void BEEP_Init(void);

#endif
