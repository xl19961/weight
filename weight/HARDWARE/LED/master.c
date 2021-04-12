#include "master.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "uart1.h"	  
#include "usart_op.h"
u32 RS485_Baudrate=9600;//ͨѶ������
u8 RS485_Parity=0;//0��У�飻1��У�飻2żУ��
u16 RS485_Frame_Distance=4;//����֡��С�����ms),������ʱ������Ϊ����һ֡ �ڴ�Ϊtim7���������ʱ

u8 RS485_RX_BUFF[2048];//���ջ�����2048�ֽ�
u16 RS485_RX_CNT=0;//���ռ�����
u8 RS485_RxFlag=0;//����һ֡�������

u8 RS485_TX_BUFF[2048];//���ͻ�����
u16 RS485_TX_CNT=0;//���ͼ�����
u16 RS485_TxFlag=0;//����һ֡�������



/////////////////////////////////////////////////////////////////////////////////////
//����������
u8   SlaverAddr=0x01;    //�ӻ���ַ
u8   Fuction=0x03;      // ������
u16  StartAddr=0xC10C;    //��ʼ��ַ
u16  ValueOrLenth=0x01;  //����or����

u8 message=0;
//////////////////////////////////////////////////////////////////////////////////////////

u8 TX_RX_SET=0; //���ͣ����������л��� 0 ����ģʽ 1����ģʽ    
u8 ComErr=8; //0����ͨѶ����
             //1����CRC����
			       // 2����������� ����Ĭ�ϵ���0x03
						 //7ΪͨѶ��ʱ
						 //8����δ��ʼ
						 //
u8 errpace=0;	//1����tim7���ڴ���ʱ���������������֡
               // 2��⵽������֡�����У�����
			
u8 state=0;


//��ʼ��USART2
void RS485_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PA2��TX�������������
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_SetBits(GPIOA,GPIO_Pin_2);//Ĭ�ϸߵ�ƽ
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//PA3��RX����������
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //�޸�ԭGPIO_Mode_IPU������������->GPIO_Mode_IN_FLOATING(��������)/////////////////////////////////////////////
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//�޸�PG9��RE/DE��ͨ���������->PD7��RE/DE��ͨ���������//////////////////////////////////////////////////////////////////////
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_ResetBits(GPIOA,GPIO_Pin_1);//Ĭ�Ͻ���״̬
        
        USART_DeInit(USART2);//��λ����2
        USART_InitStructure.USART_BaudRate=RS485_Baudrate;
        USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
        USART_InitStructure.USART_WordLength=USART_WordLength_8b;
        USART_InitStructure.USART_StopBits=USART_StopBits_1;
        USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//�շ�ģʽ
        switch(RS485_Parity)
        {
                case 0:USART_InitStructure.USART_Parity=USART_Parity_No;break;//��У��
                case 1:USART_InitStructure.USART_Parity=USART_Parity_Odd;break;//��У��
                case 2:USART_InitStructure.USART_Parity=USART_Parity_Even;break;//żУ��
        }
        USART_Init(USART2,&USART_InitStructure);
        
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܴ���2�����ж�
        
        NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART2,ENABLE);//ʹ�ܴ���2
        RS485_TX_EN=1;//Ĭ��Ϊ����ģʽ
        
        Timer3_Init();//��ʱ��7��ʼ�������ڼ��ӿ���ʱ��
        //Modbus_RegMap();//Modbus�Ĵ���ӳ��
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//��ʱ��4��ʼ��
//��ʱ1s����ͨѶ����2����ǰ500ms���з��͹��ܣ���500ms����ӻ����ص�����
void Timer4_enable(u16 arr)	   	//TIM4ʹ��
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM4, //TIM2
		TIM_IT_Update  |  //TIM �ж�Դ
		TIM_IT_Trigger,   //TIM �����ж�Դ 
		ENABLE  //ʹ��
		);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����							 
}

void Timer4_disable (void)					   //TIM4ʧ��
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE); //ʱ��ʧ��
	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_Trigger,DISABLE );
	TIM_Cmd(TIM4, DISABLE);  //ʧ��TIMx����
}
///////////////////////////////////////////////////////////////////////////////////////////////
//��ʱ��7��ʼ��---���ܣ��жϴӻ����ص������Ƿ�������

void Timer3_Init(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM7ʱ��ʹ�� 

        //TIM7��ʼ������
        TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Distance*10; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
        TIM_TimeBaseStructure.TIM_Prescaler =7199; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

        TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );//TIM7 ��������ж�

        //TIM7�жϷ�������
        NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM7�ж�
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
        NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���                                                                  
}






/////////////////////////////////////////////////////////////////////////////////////
void USART2_IRQHandler(void)//����2�жϷ������
{
	   
        u8 res;
        u8 err;
	 
        if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
        {
                if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) {err=1;errpace=2;}//��⵽������֡�����У�����
                else err=0;
                res=USART_ReceiveData(USART2); //�����յ����ֽڣ�ͬʱ��ر�־�Զ����
                
                if((RS485_RX_CNT<2047)&&(err==0))
                {
                        RS485_RX_BUFF[RS485_RX_CNT]=res;
                        RS485_RX_CNT++;
                        
                        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//�����ʱ������ж�
                        TIM_SetCounter(TIM3,0);//�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
                        TIM_Cmd(TIM3,ENABLE);//��ʼ��ʱ
                }
        }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	if (TIM_GetITStatus(TIM4,TIM_IT_Update) == SET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ ;
		//modbus_rtu();
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////

//�ö�ʱ��7�жϽ��տ���ʱ��,������ʱ�����ָ��ʱ�䣬��Ϊһ֡����
//��ʱ��7�жϷ������         
void TIM3_IRQHandler(void)
{                                                                   
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//����жϱ�־
		TIM_Cmd(TIM3,DISABLE);//ֹͣ��ʱ��
		RS485_TX_EN=1;//ֹͣ���գ��л�Ϊ����״̬
		RS485_RxFlag=1;//��λ֡�������
		errpace=1;
	}
}

//////////////////////////////////////////////////////////////////////////////
//����n���ֽ����� ���������ݽ��з���
//buff:�������׵�ַ
//len�����͵��ֽ���
void RS485_SendData(u8 *buff,u8 len)
{ 
		RS485_TX_EN=1;//�л�Ϊ����ģʽ
		while(len--)
		{
						while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//�ȴ�������Ϊ��
						USART_SendData(USART2,*(buff++));
		}
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);//�ȴ��������
		TX_RX_SET=1; //����������ɣ���ʱ��T4������յ�������
		RS485_TX_EN=0;
}



//д��Ƶ������ָֹͣ��
void WRITE_CF(u8 value)
{
	  RS485_TX_BUFF[0]=0x25;//%:��ʾ��Ϣ�Ŀ�ʼ
		RS485_TX_BUFF[1]=0x30;
	  RS485_TX_BUFF[2]=0x31;//վ��'01'
		RS485_TX_BUFF[3]=0x23;//#����ʾָ����Ϣ
		RS485_TX_BUFF[4]=0x57;//W
	  RS485_TX_BUFF[5]=0x43;//C-------WC���ӵ�д��
	  RS485_TX_BUFF[6]=0x53;//S�����㴦��
	  RS485_TX_BUFF[7]=0x52;//C�����������
	  RS485_TX_BUFF[8]=0x32;
	  RS485_TX_BUFF[9]=0x35;
		RS485_TX_BUFF[10]=0x30;
	  RS485_TX_BUFF[11]=0x30;//'2500'�̵�����ַ
		RS485_TX_BUFF[12]= value;//'1'������'0'ֹͣ
		RS485_TX_BUFF[13]=0x2A;
	  RS485_TX_BUFF[14]=0x2A;//BCCУ����
	  RS485_TX_BUFF[15]=0x0D;//cr������
		RS485_SendData(RS485_TX_BUFF, 16);
}

//д��Ƶ��Ƶ��ָ��
void WRITE_CF_HZ(u8 value)
{
	u16 hz = value * 0x42;
	u16 high,low;
	high = HexToASCII((hz>>8)&0xff);
	low = HexToASCII(hz&0xff);
	  RS485_TX_BUFF[0]=0x25;//%:��ʾ��Ϣ�Ŀ�ʼ
		RS485_TX_BUFF[1]=0x30;
	  RS485_TX_BUFF[2]=0x31;//վ��'01'
		RS485_TX_BUFF[3]=0x23;//#����ʾָ����Ϣ
		RS485_TX_BUFF[4]=0x57;//W
	  RS485_TX_BUFF[5]=0x44;//D
	  RS485_TX_BUFF[6]=0x44;//D:д���ݼĴ���
	  RS485_TX_BUFF[7]=0x30;
	  RS485_TX_BUFF[8]=0x30;
	  RS485_TX_BUFF[9]=0x32;
		RS485_TX_BUFF[10]=0x33;
	  RS485_TX_BUFF[11]=0x38;//��ʼ��ַ��R238
		RS485_TX_BUFF[12]=0x30;
	  RS485_TX_BUFF[13]=0x30;
	  RS485_TX_BUFF[14]=0x32;
		RS485_TX_BUFF[15]=0x33;
	  RS485_TX_BUFF[16]=0x38;//������ַ��R238
		RS485_TX_BUFF[17]=(low>>8)&0xff;//
		RS485_TX_BUFF[18]=low&0xff;
		RS485_TX_BUFF[19]=(high>>8)&0xff;
		RS485_TX_BUFF[20]=high&0xff;
		RS485_TX_BUFF[21]=0x2A;
	  RS485_TX_BUFF[22]=0x2A;//BCCУ����
	  RS485_TX_BUFF[23]=0x0D;//cr������
		RS485_SendData(RS485_TX_BUFF, 24);
}

//ʮ������תASCII��
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


