#ifndef __SHOW_H
#define __SHOW_H
#include "sys.h"
#include "oled.h"
#include "key.h"
#include "led.h"
#include "pub_sub_message.h"
extern u8 soil;
extern float tem;
extern int humi;
extern u8 tem_up_2;  //空气温度阈值
extern u8  mq123_up_2;//气体浓度阈值
extern u8  land_down_2;//土壤湿度阈值

extern u8  tem_down; 
extern u8  soil_down, soil_up; 
extern u8  humi_up, humi_down; //存储湿度上下限值


extern u8 auto_flag;
extern u8 hand_flag,wifi_flag;

extern u8 wifi_connect;
extern u8 APP_alarm;//app报警


void main_screen(void);//主界面

#endif
