#ifndef __UART_H
#define __UART_H
#include "stdio.h"	
#include "sys.h" 
#include "stm32f10x.h"
#include "usart_op.h"

#define		UART_BufLen						0x01ff


#define		USARTx1								USART1
#define 	USART_APBCLK					RCC_APB2Periph_USART1
#define 	USARTx_IRQn						USART1_IRQn
#define		USARTx_IRQHandler			USART1_IRQHandler


#define		UARTx1_TxBuf				u1s_buf
#define		UARTx1_TxIndex			u1s_index
#define		UARTx1_TxPushIndex 	u1s_push_index

#define		UARTx1_RxBuf				r1s_buf
#define		UARTx1_RxIndex			r1s_index
#define		UARTx1_RxPushIndex 	r1s_push_index
extern  USART_BufDef USART1_Rec;




//如果想串口中断接收，请不要注释以下宏定义
void uart1_init(u32 bound);
void USART1_Push(unsigned char* str,unsigned char lth);
void USART1_Receive(unsigned char *buf,unsigned char *len);
void READ_HMI(u16 address,u8 lenth);
void CLOSE_OPEN_CF(u8 hmi_key_value);
void INFO1(unsigned char* format,...);
#endif

