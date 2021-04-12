#include "ad7191.h"
#include "FlagDeal.h"
#include "math.h"
unsigned int data;
float amp;
float er=0,amp_pre=0 ;
//float value;
//float filter_value;
//unsigned long FullScale;
//unsigned long Data[N];
//AD7191引脚初始化
void AD7191_gpio_Configuration()
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTB时钟使能  	 	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PA5/6/7复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOb
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5;//CHANNAL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //PA5/6/7复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOb
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ODR1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //PA5/6/7复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOb
	
	PAout(5) = 1;
}
//开启AD
void ad7191_on()
{
	pw_on;
	delay_ms(80);
}
//关闭AD
void ad7191_off()
{
	pw_down;
	delay_ms(80);
}
//AD读出数据
unsigned int read_data_from_ad7191()
{
	char i;
	unsigned int  data = 0;
	for (i = 0; i < 24; i++){
		PAout(5) = 0;
		data = data << 1;
		data |= PAin(6);
		PAout(5) = 1;
		//delay_us(1);
	}
	return data;
}
//选择AD通道
void sellect_channal(int channal)
{
	if(channal == 1)
		ad7191_channal1(0);
	else
		ad7191_channal1(1);
}
//设置AD输出速率
void set_rate(enum rate_switch rate)
{
	switch (rate){
		case rate_10://11
			ad7191_odr1(1);
			ad7191_odr2(1);
			break;
		case rate_50://01
			ad7191_odr1(0);
			ad7191_odr2(1);
			break;
		case rate_60://00
			ad7191_odr1(0);
			ad7191_odr2(0);
			break;
		case rate_120://10
			ad7191_odr1(1);
			ad7191_odr2(0);
			break;
		default://01
			ad7191_odr1(0);
			ad7191_odr2(1);
			break;
	}
}
//设置PGA增益
void set_gain(enum gain_switch gain)
{
	switch (gain){
		case  gain_1://00
			ad7191_gain1(0);
			ad7191_gain2(0);
			break;
		case gain_8://10
			ad7191_gain1(1);
			ad7191_gain2(0);
			break;
		case gain_64://01
			ad7191_gain1(0);
			ad7191_gain2(1);
			break;
		case gain_128://11
			ad7191_gain1(1);
			ad7191_gain2(1);
			break;
		default://00
			ad7191_gain1(0);
			ad7191_gain2(0);
			break;

	}
}
//内部温度传感器选择
void select_tem(unsigned char tmp_select)
{
	if(tmp_select == INPUT){
		ad7191_temp(0);
	}else if(tmp_select == TEMP){
		ad7191_temp(1);
	}else{
		ad7191_temp(0);
	}
}	
//选择时钟来源（外部或内部）
void select_IN_OR_EX(unsigned char in_or_ex_select)
{
	if(in_or_ex_select == INTERNAL){
		ad7191_clksel(1);
	}else if(in_or_ex_select == EXTERNAL){
		ad7191_clksel(0);
	}else{
		ad7191_clksel(1);
	}
}
//开机前取平均
float average = 0;
unsigned char initflag = 0;
float average_ad(void)
{	
	unsigned	int data;
	static float amp = 0;
	static int i = 0;
	if(i < 100){
		while(PAin(6) == 1){
			;
		}
		data = read_data_from_ad7191();
		amp +=(float)((data)-8388608)*4.81/(128*8388608)*1000;
		i++;
		return 0;
	}else{
		amp /= 100;
		return amp;
	}
}
void average_deal(void)
{
	if( (average = average_ad()) != 0)
			initflag = 1;
}

//限幅滤波法
float filter_AD(void)
{
  float new_value;
	new_value = read_data_from_ad7191();	
  if ((new_value>1)&&(((new_value - amp)>=A)||((amp - new_value)>=A)) )
		return amp;
	else 
		return new_value;
}

//平均滤波
//unsigned long GetValueTui(void)
//{
//	u8 i;
//	unsigned long Value;
//	unsigned long Sum;
//	Sum = 0;
//	while(PAin(6) == 1){
//		;
//	}
//	Data[N] = read_data_from_ad7191(); 
//	for(i=0; i<N; i++)
//	{
//		Data[i] = Data[i+1];
//		Sum += Data[i];
//	}
//	Value = Sum/N;
//	return Value;
//}

//平均值内滤波法
unsigned long	GetValueMidTui(void)
{
	u32 sum;
	u8 i;
	while(PAin(6) == 1){
		;
	}
  data = read_data_from_ad7191();
	sum = 0;
	for(i=0; i<N; i++)
	{
  sum += data;
	}
	return (unsigned long)(sum/N);
}

//求数据长度
int deal_num(unsigned char *deal_tmp, unsigned char *data)
{
	int i = 0;
	while(*data != 0x00){  //未结束
		if((i % 2) == 0)
			deal_tmp[i++] = 0X00;
		else{
			deal_tmp[i] = *data;
			i++;    
			data++;//指向下个字符数组
		}
	}
	return i;
}
unsigned char light_sensor = 0;
//AD值处理和发送
void get_data_and_send(void)
{
	static float sum = 0;
	static int count = 0;
	static float damp;
		static int i_con=0;
	
	unsigned char *printbuf;
	unsigned char *data_print;
	unsigned char *pp;
	int length = 0;
//	while(PAin(6) == 1){
//		;
//	}
//  data = read_data_from_ad7191();
  data = GetValueMidTui();
	amp =(float)((data)-8388608)*4.81/(128*8388608)*1000;
//amp=2000*(amp-average)/(4.708-average);
	
	amp= 2865.33*amp -11512.89;//传感器拟合直线
	
  if(amp<=3)
  { 
//		amp/=10000;
//	
//		amp=fabs(amp);
		amp = 0;
  }
	
//    
//		
		
	if (count % 8 == 0)
	 {
		 
		 	amp = sum;
			i_con ++;
		 
			printbuf = (unsigned char *) calloc (40,sizeof(unsigned char));
			data_print = (unsigned char *) calloc (20,sizeof(unsigned char));
			pp = (unsigned char *) calloc (40,sizeof(unsigned char));
			if(flag_state == 2)
			{
				if (light_sensor == 0){
					amp = 0.0;
				}else{
					light_sensor--;
					sprintf((char *)data_print,"%.2f\r\n",amp);
				}
			
			}
			else
				{
					if(i_con ==5)
					{
						if(amp>2)
						{
							er= amp-amp_pre ;
							if(fabs(er)<2)
							{
							sprintf((char *)data_print,"%.2f\r\n",amp_pre);}
							
							
							else
							{
								sprintf((char *)data_print,"%.2f\r\n",amp);
								amp_pre = amp;
							}}
							else
								{
									amp=0;
									sprintf((char *)data_print,"%.2f\r\n",amp);
									
							}
						
						
						i_con =0;
					}
			}
		 
			length = deal_num(pp,data_print);
			USART3_Push(data_print,length);       //调试串口
			printbuf[0] = 0x5a;
			printbuf[1] = 0xa5;
			printbuf[2] = length + 3;
			printbuf[3] = 0x82;
			printbuf[4] = (ADDR & 0xff00) >> 8;
			printbuf[5] = (ADDR & 0x00ff);
			memcpy((printbuf + 6),pp,length);
			USART1_Push(printbuf,length + 6);    //RS232给串口屏
			free(data_print);
			free(printbuf);
			free(pp);
		
		count = 0;
	 }
	 else
		 sum = ( sum + amp ) / 2;
	 
	 ++count;
}
//AD配置函数
void ad7191_config(enum rate_switch rate, enum gain_switch gain, int channal, unsigned char in_or_temp, unsigned char in_or_ex)
{
	ad7191_off();
	ad7191_on();
	AD7191_gpio_Configuration();
	sellect_channal(channal);
	set_rate(rate);
	select_tem(in_or_temp);
	select_IN_OR_EX(in_or_ex);
	set_gain(gain);
	//delay_ms(450);
}

