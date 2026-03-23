#ifndef _NBIOT_H_
#define _NBIOT_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

extern char NBIOT_buf[300];
extern unsigned short NBIOT_cnt;

extern uint8_t RecOK;


void NBIOT_Init(void);

void NBIOT_Clear(void);

void NBIOT_SendData(void);

_Bool NBIOT_SendCmd(char *cmd, char *res);

void MqttUpData(void);

void MqttRecHandle(void);

void UpdataSetData(void);

void NBIOT_SendSetInitData(void);

#endif
