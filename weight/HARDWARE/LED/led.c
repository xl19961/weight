#include "led.h"
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 
RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTBʱ��ʹ��  	 	
 RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB13/14/15����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOB

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PA5/6/7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOb
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;//CHANNAL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //PA5/6/7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOb
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ODR1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //PA5/6/7����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOb
	
}
