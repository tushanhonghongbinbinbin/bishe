#include "hardware.h"
#include "delay.h"
#include "MAX30100_Filters.h"
#include "MAX30100_PulseOximeter.h"
#include "MAX30100_SpO2Calculator.h"
#include "stdlib.h"
#include "string.h"

int16_t AX, AY, AZ, GX, GY, GZ;			//定义用于存放各个数据的变量
extern int TestTime;
extern uint16_t VoiceTime;
uint8_t SIZE_X = 16;   // 字体宽度
uint8_t SIZE_Y = 16;   // 字体高度

float tem;
extern u8  ID;

uint8_t KeyWarning = 0;//按键报警

T_SVARIBAL s_tVariable = {30,40};

uint8_t NBIOTConnectFlag = 0;//是否进入配网

uint8_t WarningCount;

uint8_t SendMessageFlag = 0;


char PheneNum[11] = "18878419723";
extern u8  ID;
//主函数
int main(void)
 {

	Delay_Init();		 					// 延时功能初始化
	Delay_Ms(2000);		
  HardWare_Init(); 						// 硬件初始化

	ID=Read_One_Byte(MAX30100_REG_PART_ID);//获取ID
	if(ID==17)
	{
		Delay_Ms(1000);
		setMode(MAX30100_MODE_SPO2_HR);																 //使能SPO2 HR 模式
		setLedsPulseWidth(MAX30100_SPC_PW_1600US_16BITS);							 //LED设置16bit脉冲宽度 AD
		setSamplingRate(MAX30100_SAMPRATE_100HZ);											 //SpO2采样率设置 100HZ
		setLedsCurrent( MAX30100_LED_CURR_24MA,MAX30100_LED_CURR_24MA);//24mA 设置红外LED的电流大小
		setHighresModeEnabled(1);	//高分辨率采样使能SpO2
		BeatDetector();						//比特检测器
		setOnBeatDetectedCallback(&onBeatDetected);				//比特检测器回拨
		begin(PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT);		//开始脉搏血氧计调试模式脉搏检测
	}				 
	 
	 
	OLED_ShowString(SIZE_X*0,SIZE_Y*0,"Net>>?",16,1);
	OLED_Refresh();
	while(1)
	{
		if(Key_Up == 0)
		{
			OLED_Clear();
			OLED_ShowString(SIZE_X*0,SIZE_Y*0,"Connect...",16,1);
			NBIOTConnectFlag = 1;
			OLED_Refresh();
			break;
		}
		if(Key_Power == 0)
		{
			NBIOTConnectFlag = 0;
			break;
		}
	}
	if(NBIOTConnectFlag == 1)
	{
		NBIOT_Init();//连接MQTT服务器
	}	
	OLED_Clear();
	Delay_Ms(1000);
	Delay_Ms(1000);
	RecOK = 0;
	s_tKeyEvent.us_UIPage = 0;

	while (1)			   					// 主循环
	{
		if(NBIOTConnectFlag == 1)
		{
			if(RecOK != 1)
			{
				if(MQTTBaseTime > 3000)
				{//3秒更新一次数据
					MQTTBaseTime = 0;
					MqttUpData();
				}
			}
			else
			{
				MQTTBaseTime = 0;
				MqttRecHandle();
			}
		}
			if(s_tKeyEvent.us_UIPage == 0)
			{			
					if(s_tVariable.HeartMeasureKey == 1)
					{
						POupdate();				//更新FIFO数据 血氧数据 心率数据
					}
					else
					{
						HeartRate = 0;
					}
					MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);		//获取MPU6050的数据
					if(TestTime > 500)
					{
						TestTime = 0;
						get_value_Sunsine();//获取光照强度
						get_value_Water();//监测水
						
						parseGpsBuffer();//获取GPS位置信息 经度纬度
						printGpsBuffer();
						
						
						if(s_tVariable.HeartMeasureKey == 1)
						{
							OLED_ShowNum(80,16,HeartRate,3,16,1);
							OLED_ShowString(104,16,"BPM",16,1);
						}
						else
						{
							OLED_ShowString(80,16,"Close ",16,1);
						}
						
						s_tVariable.MeasureDistance = sonar_mm()/10;//测距
						
						s_tVariable.f_temp = (float)DS18B20_Get_Temp()/10;
						
						//前方障碍物过近
						if(s_tVariable.MeasureDistance < s_tVariable.SetDistance)
						{
							BEEP_ON;
						}
						else
						{
							BEEP_OFF;
						}
						
						if(s_tVariable.FallDownFlag == 0)
						{
							if(s_tVariable.adc_Water > 20)
							{
								s_tVariable.WaterFlag = 1;
								//检测到水
								if(VoiceTime > 4000)
								{
									VoiceTime = 0;
									VoiceSendData(0x0a);
									VoiceSendData(0x03);
									VoiceSendData(0x0b);	
								}								
							}
							else
							{
								s_tVariable.WaterFlag = 0;
							}
						}
						if(AX >= 1000||AX <= -1000||AY >= 1000 ||AY <= -1000 )//进入报警模式
						{//摔倒模式
							WarningCount++;
							if(WarningCount > 10)
							{
								s_tVariable.FallDownFlag = 1;//摔倒标志位
							}
						}
						else
						{
							WarningCount = 0;
							s_tVariable.FallDownFlag = 0;
						}		
						if(s_tVariable.SOS_MessageKey == 0)
						{
							if( s_tVariable.FallDownFlag == 1)
							{
								//播报 当前老人跌倒，需要帮助
								if(VoiceTime > 4000)
								{
									VoiceTime = 0;
									VoiceSendData(0x0a);
									VoiceSendData(0x01);
									VoiceSendData(0x0b);	
								}
								if(s_tVariable.FallDownFlag == 1)
								{
									if(SendMessageFlag != 2)
									{
										SendMessageFlag = 2;
										//发送摔倒报警信息
										sim900a_send_Chinese_message("ch",PheneNum,0);
										sim800c_send_English_message("en",PheneNum);
									}
								}
								
							}
							else
							{
								if(SendMessageFlag != 0)
								{
									SendMessageFlag = 0;
									sim900a_send_Chinese_message("ch",PheneNum,2);
								}
							}
						}
						else
						{
								if(VoiceTime > 4000)
								{
									VoiceTime = 0;
									VoiceSendData(0x0a);
									VoiceSendData(0x02);
									VoiceSendData(0x0b);	
								}
						}
						if(s_tVariable.FallDownFlag == 1)
						{
							OLED_ShowString(120,32,"F",16,1);
							//OLED_ShowChinese(88,0,10,16,1);//摔倒
							//OLED_ShowChinese(104,0,11,16,1);	
						}
						else
						{
							OLED_ShowString(120,32,"N",16,1);
							//OLED_ShowChinese(88,0,8,16,1);//正常
							//OLED_ShowChinese(104,0,9,16,1);		
						}			
						
						
						OLED_ShowChinese(0,0,4,16,1);		
						OLED_ShowChinese(16,0,5,16,1);
						OLED_ShowString(32,0,":",16,1);
						OLED_ShowNum(40,0,(int)(s_tVariable.f_temp*10)/10,2,16,1);
						OLED_ShowString(56,0,".",16,1);
						OLED_ShowNum(64,0,(int)(s_tVariable.f_temp*10)%10,1,16,1);
						OLED_ShowString(72,0,"C",16,1);
						
						OLED_ShowNum(90,0,s_tVariable.MeasureDistance,3,16,1);
						OLED_ShowString(114,0,"cm",16,1);
						//显示光照强度
						OLED_ShowChinese(0,16,0,16,1);	
						OLED_ShowChinese(16,16,1,16,1);	
						OLED_ShowString(32,16,":",16,1);
						OLED_ShowNum(40,16,s_tVariable.adc_Sunsine,2,16,1);
						OLED_ShowString(56,16,"%",16,1);
						//光照强度小于阈值时，打开照明灯
						if(s_tVariable.adc_Sunsine < s_tVariable.SetSunsine)
						{
							LightLed_ON;
						}
						else
						{
							LightLed_OFF;
						}
						
						//显示GPS
						OLED_ShowChinese(0,32,6,16,1);//经度		
						OLED_ShowChinese(16,32,5,16,1);//经度		
						OLED_ShowString(32,32,buff1,16,1);
						
						OLED_ShowChinese(0,48,7,16,1);//维度		
						OLED_ShowChinese(16,48,5,16,1);//维度		
						OLED_ShowString(32,48,buff2,16,1);	
					}
					
				}
			//按键任务
				Key_Task();

				OLED_Refresh();

	}
}
