#include "sys.h"
#include "stdio.h"
#include "uart1.h"	
#include"master.h"
 unsigned char 	USART1SendBuf[256];
 unsigned char	USART1SendBufLen = 0;
 unsigned char  USART1RecieveBuf[256];     	//接收缓冲
 unsigned char	USART1RecieveBufLen = 0; 	//接收到的数据长度	  
 USART_BufDef USART1_Tx;
 USART_BufDef USART1_Rec;

//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(USART_APBCLK|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
  //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

  //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
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

	USART_Init(USARTx1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USARTx1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USARTx1, ENABLE);                    //使能串口 

}



//串口1发送函数
void USART1_Push(unsigned char* str,unsigned char lth) 
{
	unsigned char push_index = 0;
	for(push_index = 0; push_index < lth; push_index++){
		USART1_Tx.Buf[USART1_Tx.pushIndex++] = str[push_index];
	}
	USART_ITConfig(USARTx1,USART_IT_TC,ENABLE);
}


//串口1中断服务程序
void USARTx_IRQHandler(void)                	//串口1中断服务程序
{
		
	if(USART_GetITStatus(USARTx1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART1_Rec.Buf[USART1_Rec.pushIndex++] =USART_ReceiveData(USARTx1);//(USART1->DR);	//读取接收到的数据
	} 
	if(USART_GetITStatus(USARTx1,USART_IT_TC) != RESET){
		if(USART1_Tx.Index != USART1_Tx.pushIndex){
			USART_SendData(USARTx1,USART1_Tx.Buf[USART1_Tx.Index++]);
		}else{                                                
			USART_ITConfig(USARTx1,USART_IT_TC,DISABLE);
		}
	}

} 
//读串口屏寄存器地址函数
void READ_HMI(u16 address,u8 lenth)
{
		USART1SendBuf[0]=0x5a;
		USART1SendBuf[1]=0xa5;
		USART1SendBuf[2]=0x04;
		USART1SendBuf[3]=0x83;
		USART1SendBuf[4]=((address>>8)&0xff);
		USART1SendBuf[5]=(address&0xff);
		USART1SendBuf[6]=lenth;//01
		USART1_Push(USART1SendBuf,7);
}

 //void USART1_Receive(unsigned char *buf,unsigned char *len)
//{
//	u8 rxlen= USART1RecieveBufLen;
//	u8 i=0;
//	*len=0;				//默认为0
//	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
//	if(rxlen==USART1RecieveBufLen&&rxlen)//接收到了数据,且接收完成了
//	{
//		for(i=0;i<rxlen;i++)
//		{
//			buf[i]=USART1RecieveBuf[i];	
//		}		
//		*len=USART1RecieveBufLen;	//记录本次数据长度
//		USART1RecieveBufLen=0;		//清零
//	}
//}






