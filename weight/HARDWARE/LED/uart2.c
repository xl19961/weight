#include "sys.h"
 #include "stdio.h"
#include "uart2.h"	
#include "stdlib.h"
 unsigned char 	USART2SendBuf[256];
 unsigned char	USART2SendBufLen = 0;
// unsigned char  Measure_flag = 0;
 USART_BufDef USART2_Tx;
 USART_BufDef USART2_Rec;
//初始化IO 串口1 
//bound:波特率
void uart2_init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USARTx2_CLKEnable;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USARTx2);  //复位串口1
  //USART1_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

	//USART1_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
  //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USARTx2, &USART_InitStructure); //初始化串口
	USART_ITConfig(USARTx2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USARTx2, ENABLE);                    //使能串口 
}
void USART2_Push(unsigned char* str,unsigned char lth) 
{
	unsigned char push_index = 0;
	for(push_index = 0; push_index < lth; push_index++){
		USART2_Tx.Buf[USART2_Tx.pushIndex++] = str[push_index];
	}
	USART_ITConfig(USARTx2,USART_IT_TC,ENABLE);
}

void USARTx2_IRQHandler(void)                	//串口1中断服务程序
{
		
	if(USART_GetITStatus(USARTx2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART2_Rec.Buf[USART2_Rec.pushIndex++] =USART_ReceiveData(USARTx2);//(USART1->DR);	//读取接收到的数据
	} 
	if(USART_GetITStatus(USARTx2,USART_IT_TC) != RESET){
		if(USART2_Tx.Index != USART2_Tx.pushIndex){
			USART_SendData(USARTx2,USART2_Tx.Buf[USART2_Tx.Index++]);
		}else{                                                
			USART_ITConfig(USARTx2,USART_IT_TC,DISABLE);
		}
	}

} 
//void INFO1(unsigned char* format,...)
//{
//	USART2SendBufLen = sprintf(USART2SendBuf,format);
//	USART2_Push(USART2SendBuf,USART2SendBufLen);
//}






