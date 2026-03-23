#include "control.h"
#include "hardware.h"




	

void Water_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
}

void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(GPIOB, &GPIO_InitStructure);            		                             
}

void Hot_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(GPIOB, &GPIO_InitStructure);             		                             
}


void Device_Task(uint8_t _dev, uint8_t _mode)
{
	if(_dev == Fan)
	{
		if(_mode == ON)
		{
			Water_ON;
		}
		else if(_mode == OFF)
		{
			Water_OFF;
		}
	}
	else if(_dev == Water)
	{
		if(_mode == ON)
		{
			Fan_ON;
		}
		else if(_mode == OFF)
		{
			Fan_OFF;
		}
	}
	else	if(_dev == Hot)
	{
		if(_mode == ON)
		{
			Hot_ON;
		}
		else if(_mode == OFF)
		{
			Hot_OFF;
		}
	}
}


void Speed_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   		                             
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
}

void DeviceControl_Init(void)
{
//	Water_Init();
//	Fan_Init();
//	Hot_Init();
	Speed_Init();
}
