#include "adc.h"
#include "stm32f10x.h"
#include "led.h"
#include "hardware.h"

__IO uint16_t ADC_ConvertedValue[NOFCHANEL] = {0};


// ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能
#define ADC_x ADC1
#define ADC_DMA_CHANNEL DMA1_Channel1
#define ADC_DMA_CLK RCC_AHBPeriph_DMA1

/**
 * @brief  ADC GPIO 初始化
 * @param  无
 * @retval 无
 */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	// 初始化 ADC IO
	GPIO_Init(GPIOA,  &GPIO_InitStructure);
	

}

/**
 * @brief  配置ADC工作模式
 * @param  无
 * @retval 无
 */
static void ADCx_Mode_Config(void)
{
   DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;
    
    //开启ADC时钟 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    //开启DMA1的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    
    //复位DMA控制器
    DMA_DeInit(DMA1_Channel1);
    
    //配置DMA1
    //外设及地址为：ADC 数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);    
    //存储器地址，实际上就是一个内部SRAM的变量
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &ADC_ConvertedValue;  
    //数据来源于外设
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;   
    //缓冲区大小为1，缓冲区大小应该等于存储器的大小
    DMA_InitStructure.DMA_BufferSize = NOFCHANEL;//修改   
    //外设寄存器只有一个，地址不用递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    
    //储存器地址递增（定义的是数组）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //修改   
    //外设数据大小为半字，即两个字节
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   
    //存储器数据大小为半字，跟外设数据大小相同
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  
    //循环传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    
    //DMA传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //禁止存储器到存储器模式，因为是外设到存储器
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    DMA_Init(DMA1_Channel1,&DMA_InitStructure);
    
    //使能DMA通道
    DMA_Cmd(DMA1_Channel1,ENABLE);
    
 
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描是针对多通道的，这里多通道------------修改
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//转换模式，是否连续转换，一直转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//触发模式，我们是用软件触发，不是用外部触发，外部触发是定时器和GPIO
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
    ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;//要转换的通道数-----------修改
    ADC_Init(ADC1, &ADC_InitStructure);
    //配置ADC时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72m,6分频=12m，小于14m 
    //配置通道的转换顺序和采样时间
		ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_55Cycles5);
		ADC_RegularChannelConfig(ADC1,ADC_Channel_5,2,ADC_SampleTime_55Cycles5);

    /*最重要一句，ADC要产生DMA请求*/
    ADC_DMACmd(ADC1,ENABLE);
    /*最重要一句，ADC要产生DMA请求*/
    
    
    
    //使能ADC转换完成中断，配置中断优先级  
//   ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);//EOC为规则通道
    //使能ADC，准备开始转换
    ADC_Cmd(ADC1,ENABLE);
    
    //校准ADC，其实有的时候不校准也不影响，是对精度的校准
    //开始校准
    ADC_StartCalibration(ADC1);
    //等待校准完成
    while(ADC_GetCalibrationStatus(ADC1));
    
    //软件触发ADC真正开始转换
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

/**
 * @brief  ADC初始化
 * @param  无
 * @retval 无
 */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
}
/*********************************************END OF FILE**********************/
