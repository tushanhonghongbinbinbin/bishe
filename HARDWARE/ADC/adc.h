#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC1输入通道（引脚）配置**************************/
#define ADC_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADC_CLK RCC_APB2Periph_ADC1

#define ADC_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd

// 转换通道个数
#define NOFCHANEL 2




extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];

/**************************函数声明********************************/
void ADCx_Init(void);

#endif /* __ADC_H */
