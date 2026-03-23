#include "MAX30100_PulseOximeter.h"
#include "timer.h"
#include "stdlib.h"

#include "myiic.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
float onBeatDetected;

PulseOximeterState state;
PulseOximeterDebuggingMode debuggingMode;
uint32_t tsFirstBeatDetected;
uint32_t tsLastBeatDetected;
uint32_t tsLastSample;
uint32_t tsLastBiasCheck;
uint32_t tsLastCurrentAdjustment;
uint8_t redLedPower;

DCRemover irDCRemover;
DCRemover redDCRemover;

u8  ID=0;
u8  TempINT=0;
u8  TempFRAC=0;

void SPO2_Init()
{
	ID=Read_One_Byte(MAX30100_REG_PART_ID);//获取ID
	if(ID==17)
	{
		Delay_Ms(40);
		Write_One_Byte(MAX30100_REG_MODE_CONFIGURATION, MAX30100_MC_TEMP_EN);//温度使能
		TempINT=Read_One_Byte	(MAX30100_REG_TEMPERATURE_DATA_INT);					 //读取温度整数部分
		TempFRAC=Read_One_Byte(MAX30100_REG_TEMPERATURE_DATA_FRAC);				   //读取温度小数部分
		Delay_Ms(100);
		
		setMode(MAX30100_MODE_SPO2_HR);																 //使能SPO2 HR 模式
		setLedsPulseWidth(MAX30100_SPC_PW_1600US_16BITS);							 //LED设置16bit脉冲宽度 AD
		setSamplingRate(MAX30100_SAMPRATE_100HZ);											 //SpO2采样率设置 100HZ
		setLedsCurrent( MAX30100_LED_CURR_24MA,MAX30100_LED_CURR_24MA);//24mA 设置红外LED的电流大小
		setHighresModeEnabled(1);	//高分辨率采样使能SpO2
		BeatDetector();						//比特检测器
		setOnBeatDetectedCallback(&onBeatDetected);				//比特检测器回拨
		begin(PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT);		//开始脉搏血氧计调试模式脉搏检测
	}	
}
//寄存器配置查看文档 http://www.51hei.com/bbs/dpj-135896-1.html
void DisplayChar(unsigned char c)
{
	while((USART1->SR&0X40)==0);//等待上一次发送完毕   
	USART1->DR=c;
}
void DisplayCurve(float ch0,float ch1)
{
	signed short temp;
	DisplayChar(0x03);
	DisplayChar(0xfc);
	
	temp=(signed short)ch0;	
	DisplayChar(temp&0x00ff);
	DisplayChar(temp>>8);
	temp=(signed short)ch1;
	DisplayChar(temp&0x00ff);
	DisplayChar(temp>>8);


	DisplayChar(0xfc);
	DisplayChar(0x03);
}
void PulseOximeter()
{ 
    state=PULSEOXIMETER_STATE_INIT;
    tsFirstBeatDetected=0;
    tsLastBeatDetected=0;
    tsLastSample=0;
    tsLastBiasCheck=0;
    tsLastCurrentAdjustment=0;
    redLedPower=((uint8_t)RED_LED_CURRENT_START);
    onBeatDetected=NULL;
}
void begin(PulseOximeterDebuggingMode mode)
{
    debuggingMode = mode;
		PulseOximeter();
		//消除Ir  Red 直流分量
		DCRemover1(DC_REMOVER_ALPHA,&irDCRemover.alpha,&irDCRemover.dcw);
		DCRemover1(DC_REMOVER_ALPHA,&redDCRemover.alpha,&redDCRemover.dcw);	
    state = PULSEOXIMETER_STATE_IDLE;//闲置状态
}
void POupdate()
{
    checkSample();//检查样本
}

float getHeartRate()
{
    return getRate();
}

uint8_t POgetSpO2()
{
    return getSpO2();
}

uint8_t getRedLedCurrentBias()
{
    return redLedPower;
}

void setOnBeatDetectedCallback(float *cb)
{
    onBeatDetected = *cb;
}

signed short HeartRate; 		//心跳速率
u8  SPO2;									 	//血氧浓度



void checkSample()//采集样本
{
			u8 beatDetected;						//检测到的跳动
			float filteredPulseValue;		//
			float irACValue;						//红外FIFO数据
			float redACValue;						//红光FIFO数据
			
			if (millis() - tsLastSample > 1.0 / SAMPLING_FREQUENCY * 1000.0) //距离上次采集的间隔时间大于10ms
			{
        tsLastSample = millis();		//更新计数时间
			  update();										//获取未处理的红外FIFO、红外FIFO
				
				irACValue = step(rawIRValue,&irDCRemover.alpha,&irDCRemover.dcw);			//获取红外FIFO数据
			  redACValue = step(rawRedValue,&redDCRemover.alpha,&redDCRemover.dcw);	//获取红光FIFO数据
				
				filteredPulseValue = FBstep(-irACValue);			//过滤脉冲值		
			  beatDetected = addSample(filteredPulseValue);		
        if (getRate() > 0) 
				{
						
            state = PULSEOXIMETER_STATE_DETECTING;
						SPO2update(irACValue, redACValue, beatDetected);
					
						HeartRate	=	getRate();  //获取心跳速率
						SPO2			=	getSpO2();	//获取血氧浓度

				}
				else if (state == PULSEOXIMETER_STATE_DETECTING) 
				{
            state = PULSEOXIMETER_STATE_IDLE;
					  reset();
        }
        switch (debuggingMode) 			//初始化代码设置的模式为 -- 脉搏血氧计调试模式 --
				{
            case PULSEOXIMETER_DEBUGGINGMODE_RAW_VALUES:		
								DisplayCurve(rawIRValue,rawRedValue);
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_AC_VALUES:
								DisplayCurve(irACValue,redACValue);
                break;

            case PULSEOXIMETER_DEBUGGINGMODE_PULSEDETECT:
								//printf("HeartRate = %d,SPO2 = %d\r\n",HeartRate,SPO2);
								if((HeartRate>=30 && HeartRate<=300)||(SPO2>0 && SPO2<=100)){


								}else {

								}
                break;

            default:
                break;
        }
        if (beatDetected && onBeatDetected) 
				{
            onBeatDetected1();
        }
    }
}

void onBeatDetected1(void)
{
  
}
void checkCurrentBias()
{
    if (millis() - tsLastBiasCheck > CURRENT_ADJUSTMENT_PERIOD_MS) 
		{			
			  u8 changed = 0;				
        if (getDCW(&irDCRemover.dcw)-getDCW(&redDCRemover.dcw) > 70000 && redLedPower < MAX30100_LED_CURR_50MA) 
				{
            ++redLedPower;
					changed = 1;
        } 
				else if (getDCW(&redDCRemover.dcw) - getDCW(&irDCRemover.dcw) > 70000 && redLedPower > 0) 
				{
            --redLedPower;
					changed = 1;
        }			
        if (changed) 
				{
				   	setLedsCurrent(IR_LED_CURRENT, (LEDCurrent)redLedPower);
            tsLastCurrentAdjustment = millis();
        }
        tsLastBiasCheck = millis();
    }
}

