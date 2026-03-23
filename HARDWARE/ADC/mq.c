#include "hardware.h"
#include "mq.h"
#include "adc.h"
#include "math.h"

;

extern __IO uint16_t ADC_ConvertedValue[2];


int get_value(uint8_t ch)  
{
	uint32_t val = 0;
	uint8_t count;
	for (count = 0; count < 10; count++)
	{
		val += ADC_ConvertedValue[ch];
		Delay_Ms(5);
	}
	val =  (int)val / 10;
	return val;
}

void get_value_Sunsine(void)
{
	s_tVariable.adc_Sunsine = get_value(0)/41*(-1)+100;
}

void get_value_Water(void)
{
	s_tVariable.adc_Water = get_value(1)/41;
}

