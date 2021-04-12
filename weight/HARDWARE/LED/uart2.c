#include "sys.h"
 #include "stdio.h"
#include "uart2.h"	
#include "stdlib.h"
 unsigned char 	USART2SendBuf[256];
 unsigned char	USART2SendBufLen = 0;
// unsigned char  Measure_flag = 0;
 USART_BufDef USART2_Tx;
 USART_BufDef USART2_Rec;
//��ʼ��IO ����1 
//bound:������
void uart2_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USARTx2_CLKEnable;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USARTx2);  //��λ����1
  //USART1_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2

	//USART1_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
  //Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USARTx2, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USARTx2, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USARTx2, ENABLE);                    //ʹ�ܴ��� 
}
void USART2_Push(unsigned char* str,unsigned char lth) 
{
	unsigned char push_index = 0;
	for(push_index = 0; push_index < lth; push_index++){
		USART2_Tx.Buf[USART2_Tx.pushIndex++] = str[push_index];
	}
	USART_ITConfig(USARTx2,USART_IT_TC,ENABLE);
}

void USARTx2_IRQHandler(void)                	//����1�жϷ������
{
		
	if(USART_GetITStatus(USARTx2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART2_Rec.Buf[USART2_Rec.pushIndex++] =USART_ReceiveData(USARTx2);//(USART1->DR);	//��ȡ���յ�������
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






