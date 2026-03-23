#include "stm32f10x.h"  //包含需要的头文件
#include "beep.h"        //包含需要的头文件
#include "hardware.h"

/*-------------------------------------------------*/
/*函数名：初始化BEEP函数                            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;                     
    RCC_APB2PeriphClockCmd( BEEP_GPIO_RCC, ENABLE);                               
    GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;                                          
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		                             
    GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure); 
		BEEP_OFF;

}


void Voice_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;				
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

#define sda PBout(12)// PB5

void VoiceSendData(u8 addr )//发送函数。
{
    u8 i;
 
     /*发送时关掉中断，防止中断影响时序  */
    sda = 1; /*开始拉高*/
    Delay_Us ( 1000 );
    sda = 0; /*开始引导码*/
    Delay_Us ( 3200 );/*此处延时最少要大于2ms*/
    for ( i = 0; i < 8; i++ ) /*总共8位数据  */
    {
        sda = 1;
        if ( addr & 0x01 ) /*3:1表示数据位1,每个位用两个脉冲表示  */
        {
            Delay_Us ( 600 );
            sda = 0;
            Delay_Us ( 200 );
        }
        else              /*1：3表示数据位0 ,每个位用两个脉冲表示  */
        {
            Delay_Us ( 200 );
            sda = 0;
            Delay_Us ( 600 );
        }
        addr >>= 1;
    }
    sda = 1;
}







