#include "sys.h"
#include "stdio.h"
#include "uart1.h"	
#include"master.h"
 unsigned char 	USART1SendBuf[256];
 unsigned char	USART1SendBufLen = 0;
 unsigned char  USART1RecieveBuf[256];     	//���ջ���
 unsigned char	USART1RecieveBufLen = 0; 	//���յ������ݳ���	  
 USART_BufDef USART1_Tx;
 USART_BufDef USART1_Rec;

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(USART_APBCLK|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
  //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

  //Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;
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

	USART_Init(USARTx1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USARTx1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USARTx1, ENABLE);                    //ʹ�ܴ��� 

}



//����1���ͺ���
void USART1_Push(unsigned char* str,unsigned char lth) 
{
	unsigned char push_index = 0;
	for(push_index = 0; push_index < lth; push_index++){
		USART1_Tx.Buf[USART1_Tx.pushIndex++] = str[push_index];
	}
	USART_ITConfig(USARTx1,USART_IT_TC,ENABLE);
}


//����1�жϷ������
void USARTx_IRQHandler(void)                	//����1�жϷ������
{
		
	if(USART_GetITStatus(USARTx1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART1_Rec.Buf[USART1_Rec.pushIndex++] =USART_ReceiveData(USARTx1);//(USART1->DR);	//��ȡ���յ�������
	} 
	if(USART_GetITStatus(USARTx1,USART_IT_TC) != RESET){
		if(USART1_Tx.Index != USART1_Tx.pushIndex){
			USART_SendData(USARTx1,USART1_Tx.Buf[USART1_Tx.Index++]);
		}else{                                                
			USART_ITConfig(USARTx1,USART_IT_TC,DISABLE);
		}
	}

} 
//���������Ĵ�����ַ����
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
//	*len=0;				//Ĭ��Ϊ0
//	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
//	if(rxlen==USART1RecieveBufLen&&rxlen)//���յ�������,�ҽ��������
//	{
//		for(i=0;i<rxlen;i++)
//		{
//			buf[i]=USART1RecieveBuf[i];	
//		}		
//		*len=USART1RecieveBufLen;	//��¼�������ݳ���
//		USART1RecieveBufLen=0;		//����
//	}
//}






