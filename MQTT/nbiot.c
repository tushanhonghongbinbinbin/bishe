//单片机头文件
#include "hardware.h"

//网络设备驱动
#include "NBIOT.h"
//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

#define NBIOT_CONNECT		"AT+MCONNECT=0,300\r\n"

#define NBIOT_ONENET_INFO		"AT+MIPSTART=sh-3-mqtt.iot-api.com,1883\r\n"

#define NBIOT_USERCFG_INFO  "AT+MCONFIG=thingscloud,dn35280e5uth4ten,eJ4KjjsRs9\r\n"

//#define NBIOT_SET   "AT+MQTTSUB=0,\"command/send/+\",1\r\n"
#define NBIOT_SET   "AT+MSUB=attributes/push,0\r\n"

char NBIOT_buf[300];
unsigned short NBIOT_cnt = 0, NBIOT_cntPre = 0;

const char* pubtopic="attributes";

uint8_t RecOK = 0;


char MqttCtrBuf[500] = {0};
unsigned short MqttCtrLen = 0;



//==========================================================
//	函数名称：	NBIOT_SendData
//	函数功能：	发送传感器数据
//==========================================================
void NBIOT_SendData(void)
{
    char cmdBuf[800];
    
    // 新的数据格式：AT+MPUB=attributes,0,0,{\\22temperature\\22:31.6\\2C\\22humidity\\22:65}
    memset(cmdBuf, 0, sizeof(cmdBuf));


		// 按照协议格式拼接AT指令JSON数据
		sprintf(cmdBuf, "AT+MPUB=%s,0,0,{\\\\\\\\22HeartRate\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22f_temp\\\\\\\\22:%.1f\\\\\\\\2C\\\\\\\\22HeartMeasureKey\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22adc_Sunsine\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22MeasureDistance\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22SOS_MessageKey\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22FallDownFlag\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22longitude\\\\\\\\22:%.6f\\\\\\\\2C\\\\\\\\22latitude\\\\\\\\22:%.6f}\r\n",
						pubtopic,
						HeartRate,                  // 心跳速率（signed short）
						s_tVariable.f_temp,         // 温度
						s_tVariable.HeartMeasureKey,// 心率测量按键
						s_tVariable.adc_Sunsine,    // 阳光ADC值
						s_tVariable.MeasureDistance,// 测量距离
						s_tVariable.SOS_MessageKey, // SOS紧急按键
						s_tVariable.FallDownFlag,   // 跌倒标志位
						s_tVariable.longitude,      // 经度
						s_tVariable.latitude);      // 纬度
				
    while(NBIOT_SendCmd(cmdBuf, "OK"))
    {
        if(NBIOTConnectFlag == 0)
        {
            break;
        }
        Delay_Ms(500);
    }	
}

void NBIOT_SendSetInitData(void)
{
    char cmdBuf[500];
    
    memset(cmdBuf, 0, sizeof(cmdBuf));
    
		// 按照协议格式拼接AT指令（仅包含新结构体字段）
		sprintf(cmdBuf, "AT+MPUB=%s,0,0,{\\\\\\\\22SetSunsine\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22SetDistance\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22HeartMeasureKey\\\\\\\\22:%d\\\\\\\\2C\\\\\\\\22SOS_MessageKey\\\\\\\\22:%d}\r\n",
        pubtopic,
        s_tVariable.SetSunsine,    // 光照阈值（uint8_t）
        s_tVariable.SetDistance,   // 超声波阈值（uint16_t）
        s_tVariable.HeartMeasureKey,// 心率测量按键（uint8_t）
        s_tVariable.SOS_MessageKey); // SOS紧急按键（uint8_t）
    while(NBIOT_SendCmd(cmdBuf, "OK"))
    {
        if(NBIOTConnectFlag == 0)
        {
            break;
        }
        Delay_Ms(500);
    }	
}



//==========================================================
//	函数名称：	MqttUpData
//	函数功能：	MQTT数据上传
//==========================================================
void MqttUpData(void)
{
    NBIOT_SendData();//发送数据
}

// 优化后的辅助函数
// 返回值：1 表示成功找到并解析了键值对，0 表示未找到键
int extract_int_from_json(const char* json_str, const char* key, int* out_value) {
    if (json_str == NULL || key == NULL || out_value == NULL) {
        return 0; // 参数无效
    }

    char key_with_quotes[32];
    snprintf(key_with_quotes, sizeof(key_with_quotes), "\"%s\"", key);

    char* key_ptr = strstr(json_str, key_with_quotes);
    if (key_ptr == NULL) {
        return 0; // 未找到键
    }

    char* value_ptr = strchr(key_ptr, ':');
    if (value_ptr == NULL) {
        return 0; // 格式异常
    }
    value_ptr++;

    while (*value_ptr == ' ' || *value_ptr == '\t' || *value_ptr == '\n') {
        value_ptr++;
        if (*value_ptr == '\0') {
            return 0; // 格式异常
        }
    }
    
    // 如果找到了，就把值存入 out_value 指向的地址，并返回1
    *out_value = atoi(value_ptr);
    return 1;
}

void MqttRecHandle(void) {
    if (RecOK == 1) {
        MqttCtrBuf[MqttCtrLen] = '\0';
        char* json_start = strchr(MqttCtrBuf, '{');
        if (json_start == NULL) {
            goto exit; // 数据格式错误，直接退出
        }
        
        int temp_value; // 临时变量，用于接收解析结果

				// 只有当 extract_int_from_json 返回1（成功）时，才更新 s_tVariable 的值
				if (extract_int_from_json(json_start, "SetSunsine", &temp_value)) {
						s_tVariable.SetSunsine = (uint8_t)temp_value;  // 强转匹配uint8_t类型
				}
				if (extract_int_from_json(json_start, "SetDistance", &temp_value)) {
						s_tVariable.SetDistance = (uint16_t)temp_value; // 强转匹配uint16_t类型
				}
				if (extract_int_from_json(json_start, "HeartMeasureKey", &temp_value)) {
						s_tVariable.HeartMeasureKey = (uint8_t)temp_value; // 强转匹配uint8_t类型
				}

exit:
        memset(MqttCtrBuf, 0, sizeof(MqttCtrBuf));
        MqttCtrLen = 0;
        RecOK = 0;
    }
}

//==========================================================
//	函数名称：	NBIOT_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NBIOT_Clear(void)
{

	memset(NBIOT_buf, 0, sizeof(NBIOT_buf));
	NBIOT_cnt = 0;
}

//==========================================================
//	函数名称：	NBIOT_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool NBIOT_WaitRecive(void)
{

	if(NBIOT_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(NBIOT_cnt == NBIOT_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		NBIOT_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	NBIOT_cntPre = NBIOT_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}
uint8_t DelayNBIOT = 0;
unsigned char timeOut = 200;


//==========================================================
//	函数名称：	NBIOT_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool NBIOT_SendCmd(char *cmd, char *res)
{
	
	timeOut = 100;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));

	while(timeOut--)
	{
		
		if(NBIOT_WaitRecive() == REV_OK)							//如果收到数据
		{
			;//printf("%s\r\n",NBIOT_buf);
		//	printf("%s\r\n",NBIOT_buf);
			if(strstr((const char *)NBIOT_buf, res) != NULL)		//如果检索到关键词
			{
				//printf("%s\r\n",NBIOT_buf);
				//Usart_SendString(USART3,NBIOT_buf,NBIOT_cnt);
				NBIOT_Clear();									//清空缓存
				//printf("error\r\n");
				
				return 0;
			}
			
		}
			Delay_Ms(20);
	}
	
	return 1;

}



uint8_t SysCtOk = 0;
//==========================================================
//	函数名称：	NBIOT_Init
//
//	函数功能：	初始化NBIOT
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NBIOT_Init(void)
{
	
	NBIOT_Clear();
	while(NBIOT_SendCmd("AT\r\n", "OK"))
	Delay_Ms(500);
	Delay_Ms(1000);	

	;//printf("1. AT+RST\r\n");
	while(NBIOT_SendCmd("ATE0\r\n", "OK"))
	Delay_Ms(500);	
	
	;//printf("1. AT+RST\r\n");
	while(NBIOT_SendCmd("AT+RESET\r\n", "OK"))
	Delay_Ms(500);	
		
	
	;//printf("1. AT+RST\r\n");
	while(NBIOT_SendCmd("AT\r\n", "OK"))
	Delay_Ms(500);
	Delay_Ms(1000);
	

	Delay_Ms(1000);
	while(NBIOT_SendCmd("AT+CEREG?\r\n", "+CEREG: 0,1"))
	Delay_Ms(500);	

	Delay_Ms(1000);
	;//printf("2. CWMODE\r\n");
	while(NBIOT_SendCmd("AT+CSQ\r\n", "OK"))
		Delay_Ms(500);
	

	Delay_Ms(1000);
	;//printf( "5. MQTTUSERCFG\r\n");
	while(NBIOT_SendCmd(NBIOT_USERCFG_INFO, "OK"))
	{
		Delay_Ms(1000);
	}

	Delay_Ms(1000);
	Delay_Ms(1000);
	Delay_Ms(1000);
	;//printf( "6. MQTTCONN\r\n");
	while(NBIOT_SendCmd(NBIOT_ONENET_INFO, "OK"))
	{
		NBIOT_Clear();		
		Delay_Ms(1000);
	}
	Delay_Ms(1000);
	Delay_Ms(1000);
	;//printf( "6. MQTTCONN\r\n");
	while(NBIOT_SendCmd(NBIOT_CONNECT, "OK"))
	{
		NBIOT_Clear();		
		Delay_Ms(1000);
	}
	
	Delay_Ms(1000);
	while(NBIOT_SendCmd(NBIOT_SET, "OK"))
	{
		Delay_Ms(1000);
	}
	;//printf("SysBegin Init\r\n");
	
	memset(MqttCtrBuf,0,sizeof(MqttCtrBuf));
	MqttCtrLen = 0;	
	
	SysCtOk = 1;
	
	NBIOT_SendSetInitData();
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口3收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================



void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(NBIOT_cnt >= sizeof(NBIOT_buf))	NBIOT_cnt = 0; //防止串口被刷爆
		NBIOT_buf[NBIOT_cnt++] = USART2->DR;

		if(MqttCtrLen > 199)
		{
			MqttCtrLen = 0;
		}
		if(SysCtOk == 1)
		{
			if(NBIOTConnectFlag == 1)
			{
				if(MqttCtrBuf[0] == '+')
				{
					MqttCtrBuf[++MqttCtrLen] = USART2->DR;
					if(MqttCtrLen > 6 && MqttCtrBuf[MqttCtrLen] == 0x0A)
					{//+MSUB: "attributes/push",9 byte,{"KEY":1}
						
						if(strstr((const char *)MqttCtrBuf, "+MSUB:") != NULL)
						{																	 
							RecOK = 1;
						}
						//+MQTTDISCONNECTED:0 //断网标志
						else if(strstr((const char *)MqttCtrBuf, "MQTTDISCONNECTED:0") != NULL)
						{					
							if(NBIOTConnectFlag == 1)
							{
								NBIOTConnectFlag = 0;
							}
							memset(MqttCtrBuf,0,sizeof(MqttCtrBuf));
							MqttCtrLen = 0;						
						}
						else
						{
							memset(MqttCtrBuf,0,sizeof(MqttCtrBuf));
							MqttCtrLen = 0;
						}					
					}			
				}
				else
				{
					MqttCtrBuf[0] = USART2->DR;
					MqttCtrLen = 0;
				}
			}
		}
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
