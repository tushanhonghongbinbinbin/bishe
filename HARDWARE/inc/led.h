#ifndef __LED_H
#define __LED_H
#include "sys.h"


#define LED_GPIO_RCC  RCC_APB2Periph_GPIOA
#define LED_GPIO_PORT GPIOA
#define LED_GPIO_PIN  GPIO_Pin_8

#define LED_ON GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN)
#define LED_OFF GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN)

#define LightLed_ON GPIO_SetBits(GPIOB, GPIO_Pin_5)
#define LightLed_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_5)



void LightLed_Init(void);
void LED_Init(void);

#endif
