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
	delay_init();	    	 //延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RS485_Init();//硬件层通讯使能
	uart_init(9600);  //电脑通讯
	RELAY_Init();	
	flag_sys_init();
	delay_init();	    	 //延时函数初始化	 
	uart1_init(115200);
	uart3_init(115200);
	TIM2_Int_Init(719,99);//1ms 定时器
	ad7191_config(rate_120,gain_128,2,INPUT,INTERNAL);//AD7191配置
	//ad7191_config(rate_120,gain_8,2,INPUT,INTERNAL);//AD7191配置
	while(1)
	{ 	
		Flag_Deal();
		//串口接收函数打开
		if(commdIsRec_uart1(&USART1_Rec) == 1)
		{
			USART_RecDeal(USART1);
		}
		//定时读取串口屏状态
		if(flag_test == 1)
		{
    	flag_test = 0;
			READ_HMI(0X1000,0X01);//读“启动”/“停止”状态
			delay_ms(1);
			READ_HMI(0X0010,0X02);//读“皮带转速”状态
			delay_ms(1);
			READ_HMI(0X1010,0X02);//读重量“下限”状态
			delay_ms(1);
			READ_HMI(0X1012,0X02);//读重量“上限”状态
			delay_ms(1);
			READ_HMI(0X1034,0X02);//读重量“上限”状态
		}
	}
	
}