#include "stm32f10x.h"  
#include "sys.h"
#include "usart3.h"
#include <string.h>
#include <stdio.h>
#include "sim800c.h"
#include "Delay.h"

extern u8 Usart3_buff[128];     /*static和extern的区别*/
char dispbuf[64];
extern float longitude_sum, latitude_sum;
extern u8 longitude_int, latitude_int;
extern u8 buff1[30],buff2[30];

/*****************  发送字符串 **********************/
void Usart_SendString2( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/****************************************************************串口常用函数**************************************************************/

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/**************************************************************************/
//函数作用：字符串匹对函数
//函数名称：Find_char(char *a,char *b) ;
//内部参数：数组a   数组b
/**************************************************************************/
u8 Find_char(char *a,char *b)  //b为子串
{ 
  if(strstr(a,b)!=NULL)
	    return 0;
	else
			return 1;
}
/**************************************************************************/
//函数作用：发送英文短信函数
//函数名称：sim900a_send_English_message(char *message,char *phonenumber)(uint8_t number);
//内部参数：message phonenumber、
/**************************************************************************/
void sim800c_send_English_message(char *message,char *phonenumber)
{
	Usart_SendString2(USART2,"AT+CSMS=1\r\n");              
	Delay_Ms(500);	
	Usart_SendString2(USART2,"AT+CMGF=1\r\n");              
	Delay_Ms(500);	
	
	Usart_SendString2(USART2,"AT+CSMP=17,167,0,0\r\n");              
	Delay_Ms(500);
	memset(dispbuf,0,64);
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART2,dispbuf);                            
	Delay_Ms(500);
	sprintf(dispbuf,"lo:%d'%d la:%d'%d",(int)(longitude_sum*1000)/1000,(int)(longitude_sum*1000)%1000,(int)(latitude_sum*1000)/1000,(int)(latitude_sum*1000)%1000);
	Usart_SendString2(USART2,dispbuf);                             
	Delay_Ms(500);

	Usart_SendByte(USART2,0x1A);                                //结束指令
	Delay_Ms(1000);  //延时两秒
	
} 

void sim900a_send_Chinese_message(char *message,char *phonenumber,uint8_t mode)
{ 	
	//80 01 4E BA 64 54 50 12 FF 0C 8B F7 6C E8 61 0F 6B 64 4F E1 60 6F

	Usart_SendString2(USART2,"AT+CMGF=1\r\n");               //中文短信指令1
	Delay_Ms(200);	
	
	Usart_SendString2(USART2,"AT+CSMP=17,167,0,8\r\n");               //中文短信指令1
	Delay_Ms(200);
	sprintf(dispbuf,"AT+CMGS=\"%s\"\r\n",phonenumber);
	Usart_SendString2(USART2,dispbuf);                             //中文短信指令3
	Delay_Ms(300);
	//Usart_SendString2(USART3,message);                              //中文短信指令4
	if(mode == 0)
	{
/*
当前老人摔倒，请注意

 
Hex: 5F 53 52 4D 80 01 4E BA
Hex: 5F53 524D 76F2 4EBA 6454 5012 FF0C 8BF7 6CE8 610F 000D 000A		
		*/		
		Usart_SendHalfWord(USART2,0x5F53);                            
		Usart_SendHalfWord(USART2,0x524D);                              
		Usart_SendHalfWord(USART2,0x8001);                              
		Usart_SendHalfWord(USART2,0x4EBA);                             
		Usart_SendHalfWord(USART2,0x6454);                              
		Usart_SendHalfWord(USART2,0x5012);                              
		Usart_SendHalfWord(USART2,0xFF0C);                              
		Usart_SendHalfWord(USART2,0x8BF7);                               
		Usart_SendHalfWord(USART2,0x6CE8);                              
		Usart_SendHalfWord(USART2,0x610F);                              
		Usart_SendHalfWord(USART2,0x000D);                           
		Usart_SendHalfWord(USART2,0x000A);                             
	}
	else if(mode == 1)
	{
/*
[2025-11-18 22:56:53.217 T] SOS求救信息，老人遇到危险，
Hex: 0053 004F 0053 6C42 6551 4FE1 606F FF0C 8001 4EBA 9047 5230 5371 9669 FF0C		
*/	
		Usart_SendHalfWord(USART2,0x0053);                            
		Usart_SendHalfWord(USART2,0x004F);                              
		Usart_SendHalfWord(USART2,0x0053);                              
		Usart_SendHalfWord(USART2,0x6C42);                             
		Usart_SendHalfWord(USART2,0x6551);                              
		Usart_SendHalfWord(USART2,0x4FE1);                              
		Usart_SendHalfWord(USART2,0x606F);                              
		Usart_SendHalfWord(USART2,0xFF0C);                               
		Usart_SendHalfWord(USART2,0x8001);                              
		Usart_SendHalfWord(USART2,0x4EBA);                              
		Usart_SendHalfWord(USART2,0x9047);                           
		Usart_SendHalfWord(USART2,0x5230);    		
		Usart_SendHalfWord(USART2,0x5371);    		
		Usart_SendHalfWord(USART2,0x9669);    		
		Usart_SendHalfWord(USART2,0xFF0C);    		
	}
	else
	{
		/*报警解除
sss
Hex: 62A5 8B66 89E3 9664 000D 000A
		*/
		Usart_SendHalfWord(USART2,0x62A5);                            
		Usart_SendHalfWord(USART2,0x8B66);                              
		Usart_SendHalfWord(USART2,0x89E3);                              
		Usart_SendHalfWord(USART2,0x9664);                             
		Usart_SendHalfWord(USART2,0x000D);                              
		Usart_SendHalfWord(USART2,0x000A);                              
        		
	}
	 Usart_SendByte(USART2,0x1A); 
	Delay_Ms(1000);  //延时两秒

} 



