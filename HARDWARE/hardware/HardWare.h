#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "stm32f10x.h"
#include "led.h" //包含需要的头文件
#include "oled.h"
#include "oled_show.h"
#include "beep.h"
#include "timer.h"
#include "key.h"
#include "myiic.h"
#include "usart1.h"
#include "usart.h"	 
#include "gps.h"
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#include "sim800c.h"
#include "mq.h"
#include "adc.h"
#include "distance.h"
#include "usart.h"
#include "NBIOT.h"
#include "ds18b20.h"
#include "MAX30100.h"
#include "MAX30100_PulseOximeter.h"
#include "myiic.h"

extern u8 buff1[30],buff2[30];
extern u8 longitude_int, latitude_int;
extern float longitude_sum, latitude_sum;



typedef struct
{
	uint8_t SetSunsine;// = 30;//光照阈值
	uint16_t SetDistance;// = 30;//超声波阈值
	
	float f_temp;
	
	uint8_t HeartMeasureKey;
	
	uint8_t adc_Sunsine;
	uint8_t adc_Water;
	uint16_t MeasureDistance;
	
	
	uint8_t SOS_MessageKey;//SOS紧急按键
	uint8_t WaterFlag;//
	
	
	uint8_t FallDownFlag;
	
	
	//“经度”【longitude;】、“纬度”【 Latitude】
	float longitude;
	float latitude;	
	
}T_SVARIBAL;

extern T_SVARIBAL s_tVariable;


extern uint8_t NBIOTConnectFlag;
extern uint16_t MQTTBaseTime;

extern char PheneNum[11];


extern u8 USART2_buff[300];

extern uint8_t SIZE_X;   // 字体宽度
extern uint8_t SIZE_Y;   // 字体高度

void HardWare_Init(void);





#endif

