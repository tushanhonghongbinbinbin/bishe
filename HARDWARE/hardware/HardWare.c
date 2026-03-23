#include "hardware.h"

//硬件初始化
void HardWare_Init(void)
{
 	uart_init(9600); // 串口1功能初始化，波特率9600
	uart2_init(115200);
	clrStruct();		
	Voice_Init();
	BEEP_Init();
	LightLed_Init();
	TIM3_ENABLE();
	Key_GpioInit();
	HC_SR04_Init();
	
		
	IIC_Init();					//血氧浓度模块IIC-----SCLK接到“B7”脚 SDIN接到“B8”脚

	
	DS18B20_Init();
	MPU6050_Init();		//MPU6050初始化
	ADCx_Init();
	OLED_Init();		 // oled初始化
	OLED_ColorTurn(0);	 // 0正常显示，1 反色显示
	OLED_DisplayTurn(0); // 0正常显示 1 屏幕翻转显示	//LED初始化
}





