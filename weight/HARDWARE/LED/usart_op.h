#ifndef _USART_OP_H_
#define _USART_OP_H_
#include "stm32f10x.h"

typedef enum{
	STR_UNEQUALS = 0,
	STR_EQUALS,
}StrIsEquals;



#define	UART_RX_BUFF_LEN		256

typedef struct{
	unsigned char Index;
	unsigned char pushIndex;
	unsigned char commdStart;
	unsigned char Buf[256];
}USART_BufDef;

#define	USART3_TxBufLen		1024
#define USART3_TxForAND		(USART3_TxBufLen - 1)
#define USART3_ReSend_Max	3
typedef struct{
	unsigned short Index;
	unsigned short pushIndex;
	unsigned short perindex;
	unsigned char waitforRet;
	unsigned char FbFlags;
	unsigned char reSendCnt;
	unsigned char Buf[USART3_TxBufLen];
}USART_TxBufDef;



extern float origin;
extern int flag_state;
extern unsigned char over_top, below_bottom;
extern uint8_t Rx_Deal_Str[(uint16_t)(UART_RX_BUFF_LEN*0.9)];
unsigned char commdIsRec(USART_BufDef *DataStruct);
unsigned char commdIsRec_uart1(USART_BufDef *DataS);
unsigned long int Char_To_Int(unsigned char *nums,unsigned char len);
unsigned long int HexChar_To_int(unsigned char str[],unsigned char len);
StrIsEquals	str_compare(unsigned char* str1,unsigned char* str2,unsigned char lth);
void USART_RecDeal(USART_TypeDef *USARTxD);
void str_copy(unsigned char targ[], unsigned char source[], short len);
#endif //_USART_OP_H_


