#include "master.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "uart1.h"	  
#include "usart_op.h"
u32 RS485_Baudrate=9600;//通讯波特率
u8 RS485_Parity=0;//0无校验；1奇校验；2偶校验
u16 RS485_Frame_Distance=4;//数据帧最小间隔（ms),超过此时间则认为是下一帧 在此为tim7负责进行延时

u8 RS485_RX_BUFF[2048];//接收缓冲区2048字节
u16 RS485_RX_CNT=0;//接收计数器
u8 RS485_RxFlag=0;//接收一帧结束标记

u8 RS485_TX_BUFF[2048];//发送缓冲区
u16 RS485_TX_CNT=0;//发送计数器
u16 RS485_TxFlag=0;//发送一帧结束标记



/////////////////////////////////////////////////////////////////////////////////////
//主机命令区
u8   SlaverAddr=0x01;    //从机地址
u8   Fuction=0x03;      // 功能码
u16  StartAddr=0xC10C;    //起始地址
u16  ValueOrLenth=0x01;  //数据or长度

u8 message=0;
//////////////////////////////////////////////////////////////////////////////////////////

u8 TX_RX_SET=0; //发送，接受命令切换。 0 发送模式 1接受模式    
u8 ComErr=8; //0代表通讯正常
             //1代表CRC错误
			       // 2代表功能码错误 现在默认的是0x03
						 //7为通讯超时
						 //8代表未开始
						 //
u8 errpace=0;	//1代表tim7串口待机时间过长，放弃数据帧
               // 2检测到噪音、帧错误或校验错误
			
u8 state=0;


//初始化USART2
void RS485_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PA2（TX）复用推挽输出
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_SetBits(GPIOA,GPIO_Pin_2);//默认高电平
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//PA3（RX）输入上拉
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //修改原GPIO_Mode_IPU（输入上拉）->GPIO_Mode_IN_FLOATING(浮空输入)/////////////////////////////////////////////
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//修改PG9（RE/DE）通用推挽输出->PD7（RE/DE）通用推挽输出//////////////////////////////////////////////////////////////////////
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_ResetBits(GPIOA,GPIO_Pin_1);//默认接收状态
        
        USART_DeInit(USART2);//复位串口2
        USART_InitStructure.USART_BaudRate=RS485_Baudrate;
        USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
        USART_InitStructure.USART_WordLength=USART_WordLength_8b;
        USART_InitStructure.USART_StopBits=USART_StopBits_1;
        USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//收发模式
        switch(RS485_Parity)
        {
                case 0:USART_InitStructure.USART_Parity=USART_Parity_No;break;//无校验
                case 1:USART_InitStructure.USART_Parity=USART_Parity_Odd;break;//奇校验
                case 2:USART_InitStructure.USART_Parity=USART_Parity_Even;break;//偶校验
        }
        USART_Init(USART2,&USART_InitStructure);
        
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能串口2接收中断
        
        NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART2,ENABLE);//使能串口2
        RS485_TX_EN=1;//默认为接收模式
        
        Timer3_Init();//定时器7初始化，用于监视空闲时间
        //Modbus_RegMap();//Modbus寄存器映射
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//定时器4初始化
//定时1s进行通讯，分2步，前500ms进行发送功能，后500ms处理从机返回的数据
void Timer4_enable(u16 arr)	   	//TIM4使能
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(  //使能或者失能指定的TIM中断
		TIM4, //TIM2
		TIM_IT_Update  |  //TIM 中断源
		TIM_IT_Trigger,   //TIM 触发中断源 
		ENABLE  //使能
		);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设							 
}

void Timer4_disable (void)					   //TIM4失能
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE); //时钟失能
	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_Trigger,DISABLE );
	TIM_Cmd(TIM4, DISABLE);  //失能TIMx外设
}
///////////////////////////////////////////////////////////////////////////////////////////////
//定时器7初始化---功能：判断从机返回的数据是否接受完成

void Timer3_Init(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM7时钟使能 

        //TIM7初始化设置
        TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Distance*10; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
        TIM_TimeBaseStructure.TIM_Prescaler =7199; //设置用来作为TIMx时钟频率除数的预分频值 设置计数频率为10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

        TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );//TIM7 允许更新中断

        //TIM7中断分组配置
        NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM7中断
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
        NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器                                                                  
}






/////////////////////////////////////////////////////////////////////////////////////
void USART2_IRQHandler(void)//串口2中断服务程序
{
	   
        u8 res;
        u8 err;
	 
        if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
        {
                if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) {err=1;errpace=2;}//检测到噪音、帧错误或校验错误
                else err=0;
                res=USART_ReceiveData(USART2); //读接收到的字节，同时相关标志自动清除
                
                if((RS485_RX_CNT<2047)&&(err==0))
                {
                        RS485_RX_BUFF[RS485_RX_CNT]=res;
                        RS485_RX_CNT++;
                        
                        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除定时器溢出中断
                        TIM_SetCounter(TIM3,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
                        TIM_Cmd(TIM3,ENABLE);//开始计时
                }
        }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM4_IRQHandler(void)   //TIM4中断
{
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) == SET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 ;
		//modbus_rtu();
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////

//用定时器7判断接收空闲时间,当空闲时间大于指定时间，认为一帧结束
//定时器7中断服务程序         
void TIM3_IRQHandler(void)
{                                                                   
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除中断标志
		TIM_Cmd(TIM3,DISABLE);//停止定时器
		RS485_TX_EN=1;//停止接收，切换为发送状态
		RS485_RxFlag=1;//置位帧结束标记
		errpace=1;
	}
}

//////////////////////////////////////////////////////////////////////////////
//发送n个字节数据 主机将数据进行发送
//buff:发送区首地址
//len：发送的字节数
void RS485_SendData(u8 *buff,u8 len)
{ 
		RS485_TX_EN=1;//切换为发送模式
		while(len--)
		{
						while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
						USART_SendData(USART2,*(buff++));
		}
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);//等待发送完成
		TX_RX_SET=1; //发送命令完成，定时器T4处理接收到的数据
		RS485_TX_EN=0;
}



//写变频器开启停止指令
void WRITE_CF(u8 value)
{
	  RS485_TX_BUFF[0]=0x25;//%:显示消息的开始
		RS485_TX_BUFF[1]=0x30;
	  RS485_TX_BUFF[2]=0x31;//站号'01'
		RS485_TX_BUFF[3]=0x23;//#：显示指令消息
		RS485_TX_BUFF[4]=0x57;//W
	  RS485_TX_BUFF[5]=0x43;//C-------WC：接点写入
	  RS485_TX_BUFF[6]=0x53;//S：单点处理
	  RS485_TX_BUFF[7]=0x52;//C：接入点名称
	  RS485_TX_BUFF[8]=0x32;
	  RS485_TX_BUFF[9]=0x35;
		RS485_TX_BUFF[10]=0x30;
	  RS485_TX_BUFF[11]=0x30;//'2500'继电器地址
		RS485_TX_BUFF[12]= value;//'1'开启，'0'停止
		RS485_TX_BUFF[13]=0x2A;
	  RS485_TX_BUFF[14]=0x2A;//BCC校验码
	  RS485_TX_BUFF[15]=0x0D;//cr结束符
		RS485_SendData(RS485_TX_BUFF, 16);
}

//写变频器频率指令
void WRITE_CF_HZ(u8 value)
{
	u16 hz = value * 0x42;
	u16 high,low;
	high = HexToASCII((hz>>8)&0xff);
	low = HexToASCII(hz&0xff);
	  RS485_TX_BUFF[0]=0x25;//%:显示消息的开始
		RS485_TX_BUFF[1]=0x30;
	  RS485_TX_BUFF[2]=0x31;//站号'01'
		RS485_TX_BUFF[3]=0x23;//#：显示指令消息
		RS485_TX_BUFF[4]=0x57;//W
	  RS485_TX_BUFF[5]=0x44;//D
	  RS485_TX_BUFF[6]=0x44;//D:写数据寄存器
	  RS485_TX_BUFF[7]=0x30;
	  RS485_TX_BUFF[8]=0x30;
	  RS485_TX_BUFF[9]=0x32;
		RS485_TX_BUFF[10]=0x33;
	  RS485_TX_BUFF[11]=0x38;//开始地址：R238
		RS485_TX_BUFF[12]=0x30;
	  RS485_TX_BUFF[13]=0x30;
	  RS485_TX_BUFF[14]=0x32;
		RS485_TX_BUFF[15]=0x33;
	  RS485_TX_BUFF[16]=0x38;//结束地址：R238
		RS485_TX_BUFF[17]=(low>>8)&0xff;//
		RS485_TX_BUFF[18]=low&0xff;
		RS485_TX_BUFF[19]=(high>>8)&0xff;
		RS485_TX_BUFF[20]=high&0xff;
		RS485_TX_BUFF[21]=0x2A;
	  RS485_TX_BUFF[22]=0x2A;//BCC校验码
	  RS485_TX_BUFF[23]=0x0D;//cr结束符
		RS485_SendData(RS485_TX_BUFF, 24);
}

//十六进制转ASCII码
u16 HexToASCII(u8 data_hex)
{
    u8 data_ASCII_H;
    u8 data_ASCII_L;
    u16 data_ASCII;
 
    data_ASCII_H = ((data_hex >> 4) & 0x0F);
    data_ASCII_L = data_hex & 0x0F;
 
    if(data_ASCII_H <= 9)
    {
        data_ASCII_H += 0x30;
    }
    else if((data_ASCII_H >= 10) && (data_ASCII_H <= 15))
    {
        data_ASCII_H += 0x37;
    }
 
    if(data_ASCII_L <= 9)
    {
        data_ASCII_L += 0x30;
    }
    else if((data_ASCII_L >= 10) && (data_ASCII_L <= 15))
    {
        data_ASCII_L += 0x37;
    }
 
    data_ASCII = (((data_ASCII_H & 0x00FF) << 8) | data_ASCII_L);
 
    return data_ASCII;
}


