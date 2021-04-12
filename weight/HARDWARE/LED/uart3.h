#ifndef __UART3_H_
#define __UART3_H_
#include "stdio.h"	
//#include "sys.h" 
#include "stm32f10x.h"
#include "usart_op.h"

#define		UART3_BufLen						0x01ff


#define		USARTx3									USART3
#define 	USARTx3_CLKEnable				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#define 	USARTx3_IRQn						USART3_IRQn
#define		USARTx3_IRQHandler			USART3_IRQHandler


#define		UARTx3_TxBuf				u3s_buf
#define		UARTx3_TxIndex			u3s_index
#define		UARTx3_TxPushIndex 	u3s_push_index

#define		UARTx3_RxBuf				r3s_buf
#define		UARTx3_RxIndex			r3s_index
#define		UARTx3_RxPushIndex 	r3s_push_index


extern  USART_BufDef USART3_Rec;
extern unsigned char  Measure_flag;

//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(u32 bound);
void USART3_Push(unsigned char* str,unsigned char lth);

unsigned char* int_to_char (unsigned int data);
//void INFO1(unsigned char* format,...);
#endif

