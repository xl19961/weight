#include "timer.h"
#include "led.h"
#include "FlagDeal.h"
#include "ad7191.h"
#include "usart_op.h"
//#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!



//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数

//void TIM3_PWMShiftInit(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStruct;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
//	TIM_OCInitTypeDef TIM_OCInitStruct;
// 
//	
//	/**********************TIM3 GPIO??*****************************/
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
//	
//	
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	/**********************???TimBase???*************************/
//	
//	
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
////	TIM_TimeBaseInitStruct.TIM_Period =  3599;  //??????????,Frequence = 36000000/PSC/(ARR+1);5k
//  TIM_TimeBaseInitStruct.TIM_Period =  1599;  //??????????,Frequence = 36000000/PSC/(ARR+1);11k
//	TIM_TimeBaseInitStruct.TIM_Prescaler = 1;     
//	
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
//	
//	/**********************???TIM3 OC???*************************/
//	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStruct.TIM_Pulse = 0;   //?????,CCR?CNT??????
//	//TIM_OCInitStruct.TIM_Pulse = 0;   //?????,CCR?CNT??????

//	TIM_OC1Init(TIM3,&TIM_OCInitStruct);

//	TIM_Cmd(TIM3, ENABLE);
//}

void TIM2_PWMShiftInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
 
	
	/**********************TIM3 GPIO??*****************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE); //Timer3部分重映射  TIM3_CH2->PB5    
	
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	/**********************???TimBase???*************************/
	
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period =  3599;  //??????????,Frequence = 36000000/PSC/(ARR+1);
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;     
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	/**********************???TIM3 OC???*************************/
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 1800;   //?????,CCR?CNT??????
	
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);
	
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;   
  TIM_OCInitStruct.TIM_Pulse = 2820;   //???? = 180*CCR/(ARR+1)     1200
	 //TIM_OCInitStruct.TIM_Pulse = 1;   //???? = 180*CCR/(ARR+1)
	TIM_OC3Init(TIM2,&TIM_OCInitStruct);
 
	TIM_Cmd(TIM2, ENABLE);
}
unsigned char average_flag = 0;
unsigned char to_get_num = 0;
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位x
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx			
	register_flag(get_data_and_send,&to_get_num);
	register_flag(average_deal,&average_flag);
}

int ms = 0;
unsigned char flag_test = 0;
unsigned int valve = 0, valve1 = 0;
void TIM2_IRQHandler(void)   //TIM3中断
{
	static unsigned int cnt = 0,time_buf = 0,sec_2= 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除TIMx更新中断标志 
		cnt++;
		ms++;
		if(initflag == 0){
				if((cnt % 100) == 0)
					average_flag = 1;
		}
		else if(cnt >= 4){
			sec_2++;
			if(sec_2 == 2){
				sec_2 = 0;
				to_get_num = 1;
			}
			cnt = 0;
		}
		if(ms >= 100)
		{
			flag_test = 1;
			ms = 0;
		}
		if (over_top == 1){
			valve++;
			if (valve >= 900){
				GPIO_ResetBits(GPIOC,GPIO_Pin_7);
			} else if (valve >= 1000) {
				over_top = 0;
				valve = 0;
				GPIO_SetBits(GPIOC,GPIO_Pin_7); 
			}
		}
		
		if (below_bottom == 1){
			valve1++;
			if (valve1 >= 900){
				GPIO_ResetBits(GPIOA,GPIO_Pin_11);
			} else if (valve1 >= 1000) {
				below_bottom = 0;
				valve1 = 0;
				GPIO_SetBits(GPIOA,GPIO_Pin_11);  
			}
		}
				
	}
}


					
