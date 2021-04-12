//#include "sys.h"
// #include "stdio.h"
#include "uart3.h"	
#include "stdlib.h"


 unsigned char 	USART3SendBuf[256];
 unsigned char	USART3SendBufLen = 0;
 unsigned char  Measure_flag = 0;
 USART_BufDef USART3_Tx;
 USART_BufDef USART3_Rec;

//��ʼ��IO ����1 
//bound:������
void uart3_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USARTx3_CLKEnable;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USARTx3);  //��λ����1
  //USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB.10
 
	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB.11

  //Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx3_IRQn;
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

	USART_Init(USARTx3, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USARTx3, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USARTx3, ENABLE);                    //ʹ�ܴ��� 

}




void USART3_Push(unsigned char* str,unsigned char lth) 
{
	unsigned char push_index = 0;
	for(push_index = 0; push_index < lth; push_index++){
		USART3_Tx.Buf[USART3_Tx.pushIndex++] = str[push_index];
	}
	USART_ITConfig(USARTx3,USART_IT_TC,ENABLE);
}




void USARTx3_IRQHandler(void)                	//����1�жϷ������
{
		
	if(USART_GetITStatus(USARTx3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART3_Rec.Buf[USART3_Rec.pushIndex++] =USART_ReceiveData(USARTx3);//(USART1->DR);	//��ȡ���յ�������
	} 
	if(USART_GetITStatus(USARTx3,USART_IT_TC) != RESET){                                                                                                                                                                                           
		if(USART3_Tx.Index != USART3_Tx.pushIndex){
			USART_SendData(USARTx3,USART3_Tx.Buf[USART3_Tx.Index++]);
		}else{                                                
			USART_ITConfig(USARTx3,USART_IT_TC,DISABLE);
		}
	}

} 
//void INFO1(unsigned char* format,...)
//{
//	USART1SendBufLen = sprintf(USART1SendBuf,format);
//	USART1_Push(USART1SendBuf,USART1SendBufLen);
//}

unsigned char* int_to_char (unsigned int data)
{
	unsigned char *store_char;
	int i = 0,j = 0;
	unsigned char tmp;
	store_char = (unsigned char *)calloc(200,sizeof(unsigned char));
	*(store_char + i++) = data % 10 + 48;
	while((data /= 10) != 0){
		store_char[i++] = data %10 + 48;
	}
	for (j = 0 ;j < i/2;j++){
		tmp = store_char[j];
		store_char[j] = store_char[i - j - 1];
		store_char[i - j - 1] = tmp;
	}
	store_char[i++] = '\r';
	store_char[i++] = '\n';
	store_char[i] = '\0';
	return store_char;
}
	





