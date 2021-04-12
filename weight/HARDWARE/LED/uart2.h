#ifndef __UART2_H_
#define __UART2_H_
#include "stdio.h"	
#include "sys.h" 
#include "stm32f10x.h"
#include "usart_op.h"

#define		UART2_BufLen						0x01ff


#define		USARTx2									USART2
#define 	USARTx2_CLKEnable				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#define 	USARTx2_IRQn						USART2_IRQn
#define		USARTx2_IRQHandler			USART2_IRQHandler


#define		UARTx2_TxBuf				u2s_buf
#define		UARTx2_TxIndex			u2s_index
#define		UARTx2_TxPushIndex 	u2s_push_index

#define		UARTx2_RxBuf				r2s_buf
#define		UARTx2_RxIndex			r2s_index
#define		UARTx2_RxPushIndex 	r2s_push_index

   
extern  USART_BufDef USART2_Rec;
extern unsigned char  Measure_flag;

//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(u32 bound);
void USART2_Push(unsigned char* str,unsigned char lth);
//void INFO1(unsigned char* format,...);
#endif

