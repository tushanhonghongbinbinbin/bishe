#include "key.h"
#include "oled_show.h"
#include "oled.h"
#include "hardware.h"
KEY_EVENT_T s_tKeyEvent = {0};

tKey Key;

Key_Status_Machine Press_Scan_Process;

extern unsigned char TempCompany[][16];

void Key_GpioInit(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE);
	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

// GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5	

 GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5	
}



static uint8_t Read_Press_Keyvalue(void)
{
    uint8_t value = 0;

    if(Key_Power == 0)
    {
        
        value |= Power_Key;
    
    }
    if(Key_Mode == 0)
    {
        
        value |= Menu_Key;
    
    }		
    if(Key_Up == 0)
    {
        
        value |= Up_Key;
    
    }
    if(Key_Down == 0)
    {
        
        value |= Down_Key;
    
    }
		return value;
}


static uint16_t Key_Press_Scan(void)
{
	uint16_t key_return = 0;
	
	Key.Current_Value = Read_Press_Keyvalue();
	
	switch(Press_Scan_Process)
	{
		case Wait_For_Press:
			if(Key.Current_Value)
			{
				Press_Scan_Process = Key_Eliminate_Shake;
				
				Key.SaveValue = Key.Current_Value;
			}
			else
			{
				Key.Pressed_Time = 0;
			}
			break;
			
		case Key_Eliminate_Shake:
			
			Key.Current_Value = Read_Press_Keyvalue();
			
			if(Key.Current_Value == Key.SaveValue)
			{
				Press_Scan_Process = Wait_For_Release;
				Key.Pressed_Time = 0;
			}
			else
			{
				Press_Scan_Process = Wait_For_Press;
			}
			break;
			
		case Wait_For_Release:
			
			Key.Current_Value = Read_Press_Keyvalue();
		
			if(Key.Current_Value != Key.SaveValue)
			{
				Press_Scan_Process = Wait_For_Press;
				
				key_return = Key.SaveValue;
			}
			else
			{
				if(Key.Pressed_Time >= 2000)
				{
					Press_Scan_Process = Key_Verify_Releasing;
					
					key_return = (Key.SaveValue | 0x1000);
					
//					printf("key_return-%d\r\n",key_return);
					
					Key.Pressed_Time = 0;
				}
			}
		
			break;
			
		case Key_Verify_Releasing:
			
			Key.Current_Value = Read_Press_Keyvalue();
		
		
			if(Key.Current_Value != Key.SaveValue)
			{
				Press_Scan_Process = Wait_For_Press;
			}
			break;
			
		default:break;
	}
	
	return key_return;
}

static uint16_t Key_Scan(void)
{
	uint16_t Key_Value = 0;
	
	if(Key.Scan_Time > 10)
	{
		Key_Value = Key_Press_Scan();
		
		Key.Scan_Time = 0;
	}
	
	return Key_Value;
}
char phonenumbuf[11] = "13139723521";//{1,3,1,3,9,7,2,3,5,2,1};//"13139723521";

uint8_t Time_SetMemorize = 0;
uint8_t SetPhoneNumPlace = 0;

void Key_Task(void)
{
	Key.Event = Key_Scan();
	
	if(Key.Event != Key.Last_Event)
	{
		
		Key.Last_Event = Key.Event;
		switch(Key.Event)
		{
		
			case Up_Key:							
				 if(s_tKeyEvent.us_UIPage == 1)
					{
											if(SetPhoneNumPlace == 0)
											{
												phonenumbuf[0]++;
												if(phonenumbuf[0] > 0x39)
												{
													phonenumbuf[0] = 0x30;
												}
												OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,1);
												OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,0);
											}						
											else if(SetPhoneNumPlace == 1)
											{
												phonenumbuf[1]++;
												if(phonenumbuf[1] > 0x39)
												{
													phonenumbuf[1] = 0x30;
												}
												OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,1);
												OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,0);
											}
											else if(SetPhoneNumPlace == 2)
											{
												phonenumbuf[2]++;
												if(phonenumbuf[2] > 0x39)
												{
													phonenumbuf[2] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,1);
												OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,0);
											}
											else if(SetPhoneNumPlace == 3)
											{
												phonenumbuf[3]++;
												if(phonenumbuf[3] > 0x39)
												{
													phonenumbuf[3] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,1);
												OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,0);
											}
											else if(SetPhoneNumPlace == 4)
											{
												phonenumbuf[4]++;
												if(phonenumbuf[4] > 0x39)
												{
													phonenumbuf[4] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,1);
												OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,0);
											}
											else if(SetPhoneNumPlace == 5)
											{
												phonenumbuf[5]++;
												if(phonenumbuf[5] > 0x39)
												{
													phonenumbuf[5] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,1);
												OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,0);
											}
											else if(SetPhoneNumPlace == 6)
											{
												phonenumbuf[6]++;
												if(phonenumbuf[6] > 0x39)
												{
													phonenumbuf[6] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,1);
												OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,0);
											}
											else if(SetPhoneNumPlace == 7)
											{
												phonenumbuf[7]++;
												if(phonenumbuf[7] > 0x39)
												{
													phonenumbuf[7] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,1);
												OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,0);
											}
											else if(SetPhoneNumPlace == 8)
											{
												phonenumbuf[8]++;
												if(phonenumbuf[8] > 0x39)
												{
													phonenumbuf[8] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,1);
												OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,0);
											}
											else if(SetPhoneNumPlace == 9)
											{
												phonenumbuf[9]++;
												if(phonenumbuf[9] > 0x39)
												{
													phonenumbuf[9] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,1);
												OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,0);
											}
											else if(SetPhoneNumPlace == 10)
											{
												phonenumbuf[10]++;
												if(phonenumbuf[10] > 0x39)
												{
													phonenumbuf[10] = 0x30;
												}												
												OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,1);
												OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,0);
											}
					}		
				break;
			case Menu_Key:

					OLED_Clear();
					s_tKeyEvent.us_UIPage++;
					
					if(s_tKeyEvent.us_UIPage == 1)
					{//Sunsine
						OLED_ShowString(SIZE_X*0,SIZE_Y*0,"Set PhoneNum:",16,0);
						SetPhoneNumPlace = 0;
						OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[SetPhoneNumPlace], 16,0);
						OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,1);
						OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,1);
						OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,1);
						OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,1);
						OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,1);
						OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,1);
						OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,1);
						OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,1);
						OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,1);
						OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,1);						
					}						
					else if(s_tKeyEvent.us_UIPage == 2)
					{
						sprintf(PheneNum,"%s",phonenumbuf);
						s_tKeyEvent.us_UIPage = 0;
					}

				break;				
			case Down_Key:						
				  if(s_tKeyEvent.us_UIPage == 1)
					{
											if(SetPhoneNumPlace == 0)
											{
												phonenumbuf[0]--;
												if(phonenumbuf[0] < 0x30)
												{
													phonenumbuf[0] = 0x39;
												}
												OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,1);
												OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,0);
											}						
											else if(SetPhoneNumPlace == 1)
											{
												phonenumbuf[1]--;
												if(phonenumbuf[1] < 0x30)
												{
													phonenumbuf[1] = 0x39;
												}
												OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,1);
												OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,0);
											}
											else if(SetPhoneNumPlace == 2)
											{
												phonenumbuf[2]--;
												if(phonenumbuf[2] < 0x30)
												{
													phonenumbuf[2] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,1);
												OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,0);
											}
											else if(SetPhoneNumPlace == 3)
											{
												phonenumbuf[3]--;
												if(phonenumbuf[3] < 0x30)
												{
													phonenumbuf[3] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,1);
												OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,0);
											}
											else if(SetPhoneNumPlace == 4)
											{
												phonenumbuf[4]--;
												if(phonenumbuf[4] < 0x30)
												{
													phonenumbuf[4] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,1);
												OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,0);
											}
											else if(SetPhoneNumPlace == 5)
											{
												phonenumbuf[5]--;
												if(phonenumbuf[5] < 0x30)
												{
													phonenumbuf[5] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,1);
												OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,0);
											}
											else if(SetPhoneNumPlace == 6)
											{
												phonenumbuf[6]--;
												if(phonenumbuf[6] < 0x30)
												{
													phonenumbuf[6] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,1);
												OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,0);
											}
											else if(SetPhoneNumPlace == 7)
											{
												phonenumbuf[7]--;
												if(phonenumbuf[7] < 0x30)
												{
													phonenumbuf[7] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,1);
												OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,0);
											}
											else if(SetPhoneNumPlace == 8)
											{
												phonenumbuf[8]--;
												if(phonenumbuf[8] < 0x30)
												{
													phonenumbuf[8] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,1);
												OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,0);
											}
											else if(SetPhoneNumPlace == 9)
											{
												phonenumbuf[9]--;
												if(phonenumbuf[9] < 0x30)
												{
													phonenumbuf[9] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,1);
												OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,0);
											}
											else if(SetPhoneNumPlace == 10)
											{
												phonenumbuf[10]--;
												if(phonenumbuf[10] < 0x30)
												{
													phonenumbuf[10] = 0x39;
												}												
												OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,1);
												OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,0);
											}
					}			
					else
					{
						if(s_tVariable.HeartMeasureKey == 1)
						{
							s_tVariable.HeartMeasureKey = 0;
						}
						else
						{
							s_tVariable.HeartMeasureKey = 1;
						}
					}
				break;
				
			case Power_Key:
			{

				
				if(s_tKeyEvent.us_UIPage == 0)
				{
					if(s_tVariable.SOS_MessageKey == 1)
					{
						s_tVariable.SOS_MessageKey  = 0;
						sim900a_send_Chinese_message("ch",PheneNum,2);
					}
					else
					{
						s_tVariable.SOS_MessageKey  = 1;
						//发送报警信息
						sim800c_send_English_message("en",PheneNum);
						sim900a_send_Chinese_message("ch",PheneNum,1);
					}
				}
				else if(s_tKeyEvent.us_UIPage == 1)
				{
					SetPhoneNumPlace++;
					if(SetPhoneNumPlace == 1)
					{
						OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,1);
						OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,0);
					}
					else if(SetPhoneNumPlace == 2)
					{
						OLED_ShowChar(SIZE_X*1+8,SIZE_Y*2,phonenumbuf[1], 16,1);
						OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,0);
					}
					else if(SetPhoneNumPlace == 3)
					{
						OLED_ShowChar(SIZE_X*2,SIZE_Y*2,phonenumbuf[2], 16,1);
						OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,0);
					}
					else if(SetPhoneNumPlace == 4)
					{
						OLED_ShowChar(SIZE_X*2+8,SIZE_Y*2,phonenumbuf[3], 16,1);
						OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,0);
					}
					else if(SetPhoneNumPlace == 5)
					{
						OLED_ShowChar(SIZE_X*3,SIZE_Y*2,phonenumbuf[4], 16,1);
						OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,0);
					}
					else if(SetPhoneNumPlace == 6)
					{
						OLED_ShowChar(SIZE_X*3+8,SIZE_Y*2,phonenumbuf[5], 16,1);
						OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,0);
					}
					else if(SetPhoneNumPlace == 7)
					{
						OLED_ShowChar(SIZE_X*4,SIZE_Y*2,phonenumbuf[6], 16,1);
						OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,0);
					}
					else if(SetPhoneNumPlace == 8)
					{
						OLED_ShowChar(SIZE_X*4+8,SIZE_Y*2,phonenumbuf[7], 16,1);
						OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,0);
					}
					else if(SetPhoneNumPlace == 9)
					{
						OLED_ShowChar(SIZE_X*5,SIZE_Y*2,phonenumbuf[8], 16,1);
						OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,0);
					}
					else if(SetPhoneNumPlace == 10)
					{
						OLED_ShowChar(SIZE_X*5+8,SIZE_Y*2,phonenumbuf[9], 16,1);
						OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,0);
					}
					else if(SetPhoneNumPlace == 11)
					{
						SetPhoneNumPlace = 0;
						OLED_ShowChar(SIZE_X*1,SIZE_Y*2,phonenumbuf[0], 16,0);
						OLED_ShowChar(SIZE_X*6,SIZE_Y*2,phonenumbuf[10], 16,1);						
					}
								
				}				
				break;
			}
			default:break;
			
		}
	}
	
}



