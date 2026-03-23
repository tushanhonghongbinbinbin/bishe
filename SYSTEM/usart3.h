#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口3驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   


void USART2_init(u32 bound);			//串口3初始化 

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void Usart_SendString2( USART_TypeDef * pUSARTx, char *str);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);

void UART2_SendString(char* s);

uint8_t MessageDeviceCheck(char *cmd, char *res);
#endif
