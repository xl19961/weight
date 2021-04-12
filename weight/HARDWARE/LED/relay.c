#include "relay.h"
#include "ad7191.h"
#include "usart_op.h"
void RELAY_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //LED0-->PA.8 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8	
 //GPIO_ResetBits(GPIOA,GPIO_Pin_11);	
 GPIO_SetBits(GPIOA,GPIO_Pin_11);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LED1-->PD.2 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);		//������� ��IO���ٶ�Ϊ50MHz
 //GPIO_ResetBits(GPIOC,GPIO_Pin_7); 
 GPIO_SetBits(GPIOC,GPIO_Pin_7); 	
 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PA5/6/7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOb
 
	
}
