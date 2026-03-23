#ifndef __KEY_H_
#define __KEY_H_


#include "stm32f10x.h"


#define Key_Power       (GPIOC->IDR & (1 << 15))     

#define Key_Mode        (GPIOA->IDR & (1 << 0))   
  
#define Key_Up          (GPIOC->IDR & (1 << 13))

#define Key_Down        (GPIOC->IDR & (1 << 14))   


#define Power_Key           0x0001

#define Menu_Key            0x0002

#define Down_Key            0x0004

#define Up_Key              0x0008

#define Long_Power_Key      0x1001

#define Long_Menu_Key       0x1002

#define Long_Down_Key       0x1004

#define Long_Up_Key         0x1008

typedef enum
{
	Wait_For_Press,//等待按下
	Key_Eliminate_Shake,//按键消除抖动
	Wait_For_Release,//等待抬起
	Key_Verify_Releasing,//按键确认抬起
	Key_Verify_Releasing2
}Key_Status_Machine;


typedef struct tsKey{
	uint8_t IWDG_Contr;//按键复位看门狗 IWDG_Contr = 1停止喂狗
	
	uint16_t Current_Value;
	
	uint16_t SaveValue;
	
	uint16_t Pressed_Time;
	
	uint16_t Scan_Time;
	 
	uint16_t Event;
	
	uint16_t Last_Event;
	
	uint8_t  LongButtonFlag;

}tKey;


typedef struct
{

	uint8_t SetCO;
	
	uint8_t uc_SwitchFlag;
	
	uint8_t  us_UITIME;
	uint8_t  us_UISet;
	uint8_t  us_UIPage;
	
	
	uint8_t SetY;
	uint8_t SetMonth;
	uint8_t SetD;
	uint8_t SetH;
	uint8_t SetMin;
	
}KEY_EVENT_T;

extern KEY_EVENT_T s_tKeyEvent;

extern tKey Key;


void Key_GpioInit(void);

void Key_Task(void);

#endif
