#ifndef _SIM900A_H_
#define _SIM900A_H_


#define COMMUNITE_ERROR       -1 
#define NO_SIM_CARD_ERROR     -2
#define SIM_CARD_NO_REG_ERROR -3
#define CALL_ERROR			  -4
#define AT_CPMS_ERROR		  -5
#define AT_CMGF_ERROR		  -6
#define AT_CSCS_ERROR		  -7
#define AT_CMGS_ERROR         -8
#define END_CHAR_ERROR		  -9
#define AT_CGATT_ERROR   	  -11
#define AT_CGACT_ERROR	      -12
#define AT_CIPSTART_ERROR	  -13
#define AT_CIPSEND_ERROR	  -14
#define AT_CIPMUX_ERROR       -15
#define AT_CIPQSEND_ERROR     -16
#define AT_CSTT_ERROR		  -17
#define AT_CIICR_ERROR		  -18
#define AT_CGATT1_ERROR       -50
#define EAT0_ERROR            -51

void sim800c_send_English_message(char *message,char *phonenumber);

void sim900a_send_Chinese_message(char *message,char *phonenumber,uint8_t mode);

void Sim900a_Init(void);

u8 UART3_Send_AT_Command(char *b,char *a,u8 wait_time,u32 s);
u8 Wait_CREG(u8 query_times);
int check_ga6_status(void);
int send_pdu_message(char *content);

#endif


