#include "hardware.h"                
#include "Delay.h"
#include "sys.h"
#include "USART1.h"	
#include <stdlib.h>
#include "gps.h"

float longitude_sum, latitude_sum;
u8 longitude_int, latitude_int;
u8 buff1[30],buff2[30];

void errorLog(int num)
{
	
	while (1)
	{
//	  	printf("ERROR%d\r\n",num);
	}
}

void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
//		printf("**************\r\n");
//		printf(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
	}
}


void printGpsBuffer()
{
	// 转化为数字
	longitude_sum = atof(Save_Data.longitude);
	latitude_sum = atof(Save_Data.latitude);
//	 printf("维度 = %.5f %.5f\r\n",longitude_sum,latitude_sum);
	// 整数
	longitude_int = longitude_sum / 100;
	latitude_int = latitude_sum / 100;
 
	// 转化为经纬度
	longitude_sum = longitude_int + ((longitude_sum / 100 - longitude_int) * 100) / 60;
	latitude_sum = latitude_int + ((latitude_sum / 100 - latitude_int) * 100) / 60;

 
	s_tVariable.longitude = longitude_sum;
	s_tVariable.latitude = latitude_sum;	
	
	sprintf((char*)buff1,":%.3f",longitude_sum);
	sprintf((char*)buff2,":%.3f",latitude_sum);
	
//	printf("%f - %f\r\n",longitude_sum,latitude_sum);
//	if (Save_Data.isParseData)
//	{
//		Save_Data.isParseData = false;
//		
////		printf("Save_Data.UTCTime = ");
//		printf(Save_Data.UTCTime);
////		printf("\r\n");

////		if(Save_Data.isUsefull)
////		{
////			Save_Data.isUsefull = false;
////			printf("Save_Data.latitude = ");
//			printf(Save_Data.latitude);
////			printf("\r\n");


////			printf("Save_Data.N_S = ");
//			printf(Save_Data.N_S);
////			printf("\r\n");

////			printf("Save_Data.longitude = ");
//			printf(Save_Data.longitude);
////			printf("\r\n");

////			printf("Save_Data.E_W = ");
//			printf(Save_Data.E_W);
////			printf("\r\n");
//		}
//		else
//		{
////			printf("GPS DATA is not usefull!\r\n");
//		}
		
}
