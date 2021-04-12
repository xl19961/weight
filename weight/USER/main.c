#include "led.h"
#include "relay.h"
#include "delay.h"
#include "sys.h"
#include "ad7191.h"
#include "FlagDeal.h"
#include "timer.h"
#include "uart1.h"
#include "uart3.h"
#include "usart.h"
#include "master.h"
#include "usart_op.h"
#include "stdio.h"
int main(void)
{	
	SystemInit();
	delay_init();	    	 //��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RS485_Init();//Ӳ����ͨѶʹ��
	uart_init(9600);  //����ͨѶ
	RELAY_Init();	
	flag_sys_init();
	delay_init();	    	 //��ʱ������ʼ��	 
	uart1_init(115200);
	uart3_init(115200);
	TIM2_Int_Init(719,99);//1ms ��ʱ��
	ad7191_config(rate_120,gain_128,2,INPUT,INTERNAL);//AD7191����
	//ad7191_config(rate_120,gain_8,2,INPUT,INTERNAL);//AD7191����
	while(1)
	{ 	
		Flag_Deal();
		//���ڽ��պ�����
		if(commdIsRec_uart1(&USART1_Rec) == 1)
		{
			USART_RecDeal(USART1);
		}
		//��ʱ��ȡ������״̬
		if(flag_test == 1)
		{
    	flag_test = 0;
			READ_HMI(0X1000,0X01);//����������/��ֹͣ��״̬
			delay_ms(1);
			READ_HMI(0X0010,0X02);//����Ƥ��ת�١�״̬
			delay_ms(1);
			READ_HMI(0X1010,0X02);//�����������ޡ�״̬
			delay_ms(1);
			READ_HMI(0X1012,0X02);//�����������ޡ�״̬
			delay_ms(1);
			READ_HMI(0X1034,0X02);//�����������ޡ�״̬
		}
	}
	
}