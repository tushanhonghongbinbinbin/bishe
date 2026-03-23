#include "stm32f10x.h"  //包含需要的头文件
#include "led.h"        //包含需要的头文件
#include "hardware.h"

/*-------------------------------------------------*/
/*函数名：初始化LED函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( LED_GPIO_RCC, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);  
		LED_OFF;	
}

void LightLed_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
		LightLed_OFF;

}








