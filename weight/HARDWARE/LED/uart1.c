//HEADERS_PUTS
#include"stm32f10x_usart.h"
#include"uart1.h"
//HEADER_TAIL

//SUB_FUNCTIONS
static void uart1PortInit(){
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
}
                                            
static void uart1NvicInit(){
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

static void uart1PerInit(){
  
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  
}

//SUB_FUNCTIONS_TAIL

void uart1Init(){
//ASSEMBLE_HEAD
  uart1PortInit();
  uart1NvicInit();
  uart1PerInit();
//ASSEMBLE_TAIL
}

//EXTENDED_FUNCTION_HEAD
#include <stdio.h> 
#include "stdlib.h" 
#include "stdarg.h" 
 
#define UART_1_TO_3//UART_TAG_CHOOSE 
 
 
unsigned char u1sBuf[256]; 
unsigned char u1sIndex = 0; 
unsigned char u1sPindex = 0; 
 
unsigned char u1rBuf[256]; 
unsigned char u1rIndex = 0; 
unsigned char u1rGindex = 0; 
 
static void u1sPush(unsigned char* str,unsigned char lth) 
{ 
    unsigned char pushIndex = 0; 
    for(pushIndex = 0;pushIndex < lth; pushIndex++){ 
        u1sBuf[u1sPindex++] = str[pushIndex]; 
    } 
    #ifdef UART_1_TO_3 
    USART_ITConfig(USART1,USART_IT_TC,ENABLE); 
    #endif  //UART_1_TO_3 
    #ifndef UART_1_TO_3 
    USART_ITConfig(UART1,USART_IT_TC,ENABLE); 
    #endif //undef UART_1_TO_3 
} 
 
static void u1Send(void) 
{ 
    #ifdef UART_1_TO_3 
    if(u1sIndex != u1sPindex){ 
        USART_SendData(USART1,u1sBuf[u1sIndex++]); 
    }else{ 
        USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
    } 
    #endif //UART_1_TO_3 
    #ifndef UART_1_TO_3 
    if(u1sIndex != u1sPindex){ 
        USART_SendData(UART1,u1sBuf[u1sIndex++]); 
    }else{ 
        USART_ITConfig(UART1,USART_IT_TC,DISABLE); 
    } 
    #endif //undef UART_1_TO_3 
} 
 
static void u1Receive(unsigned char data) 
{ 
    u1rBuf[u1rIndex++] = data; 
} 
 
static unsigned char cal_str_lth(unsigned char* str) 
{ 
	/*1. take the most normal tech_idea 
	 *2. take a combination way (1) search 0~39	if not find "\r\n",search through 40~255 with mid method 
	*/ 
	unsigned short find_index = 0; 
	unsigned char find_buf = 0; 
	for(find_index = 0; find_index <= 255; find_index++){ 
		if((find_buf == '\r')&&(str[find_index] == '\n')) 
		{ 
			return (find_index+1); 
		} 
		find_buf = str[find_index]; 
	} 
	return 0; 
} 
 
static void str_connect(unsigned char* str1,unsigned char* str2) 
{ 
	unsigned char start_index; 
	unsigned char str2_lth; 
	unsigned char str_copy_index; 
	start_index = cal_str_lth(str1) - 2;   
	str2_lth = cal_str_lth(str2);				 
	for(str_copy_index = start_index; str_copy_index <start_index+str2_lth; str_copy_index++) 
	{ 
		str1[str_copy_index] = str2[str_copy_index - start_index]; 
	} 
} 
 
static unsigned char* uctochars(unsigned char data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 2; 
	str = (unsigned char*)calloc(5,sizeof(unsigned char)); 
	do{ 
		str[put_index] = (data%10)+0x30; 
		data /= 10; 
	}while(put_index-- != 0); 
	str[3] = '\r'; 
	str[4] = '\n'; 
	return str; 
} 
 
static unsigned char* ctochars(signed char data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 3; 
	str = (unsigned char*)calloc(6,sizeof(unsigned char)); 
	if(data < 0){ 
		data = 0 - data; 
		str[0] = '-'; 
	}else{ 
		str[0] = '+'; 
	} 
	do{ 
		str[put_index] = (data%10)+0x30; 
		data /= 10; 
	}while(put_index-- != 1); 
	str[4] = '\r'; 
	str[5] = '\n'; 
	return str; 
} 
 
static unsigned char* uhwtochars(unsigned short data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 4; 
	str = (unsigned char*)calloc(7,sizeof(unsigned char)); 
	do{ 
		str[put_index] = (data%10)+0x30; 
		data /= 10; 
	}while(put_index-- != 0); 
	str[5] = '\r'; 
	str[6] = '\n'; 
	return str; 
} 
 
static unsigned char* shwtochars(signed short data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 5; 
	str = (unsigned char*)calloc(8,sizeof(unsigned char)); 
	if(data < 0){ 
		data = 0 - data; 
		str[0] = '-'; 
	}else{ 
		str[0] = '+'; 
	} 
	do{ 
		str[put_index] = (data%10)+0x30; 
		data /= 10; 
	}while(put_index-- != 1); 
	str[6] = '\r'; 
	str[7] = '\n'; 
	return str; 
} 
 
static unsigned char* uwtochars(unsigned int data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 9; 
	str = (unsigned char*)calloc(12,sizeof(unsigned char)); 
	do{ 
		str[put_index] = (data%10)+0x30; 
		data /= 10; 
	}while(put_index-- != 0); 
	str[10] = '\r'; 
	str[11] = '\n'; 
	return str; 
} 
 
static unsigned char* swtochars(signed int data) 
{ 
	unsigned char* str; 
	unsigned char put_index = 10; 
	str = (unsigned char*)calloc(13,sizeof(unsigned char)); 
	if(data < 0){ 
		data = 0 - data; 
		str[0] = '-'; 
	}else{ 
		str[0] = '+'; 
	} 
	do{ 
		str[put_index] = (data%10) + 0x30; 
		data /= 10; 
	}while(put_index-- != 1); 
	str[11] = '\r'; 
	str[12] = '\n'; 
	return str; 
} 
 
static unsigned char* ftochars(float data) 
{ 
	unsigned char* str; 
	unsigned long int long_int_result; 
	unsigned char put_index = 9; 
	 
	str = (unsigned char*)calloc(12,sizeof(unsigned char)); 
	if(data < 0){ 
		data = 0 - data; 
		str[0] = '-'; 
	}else{ 
		str[0] = '+'; 
	} 
	long_int_result = data*10000; 
	do{ 
		str[put_index] = (long_int_result%10) + 0x30; 
		long_int_result /= 10; 
	}while(put_index-- != 6); 
	str[put_index--] = '.'; 
	do{ 
		str[put_index] = (long_int_result%10) + 0x30; 
		long_int_result /= 10; 
	}while(put_index-- != 1); 
	str[10] = '\r'; 
	str[11] = '\n'; 
	 
	return str; 
} 
 
void INFO1(unsigned char* format,...)//format是传入类型参数 
{	 
	va_list arg_ptr; 
	unsigned char para_num = 0; 
	unsigned char para_type[10];	//at most 10 parameters 
	unsigned char format_s_index; 
	unsigned char para_index; 
	//unsigned char* prt_buffer=NULL;  //字头 
	unsigned char prt_buffer_index; 
	//prt_buffer = (unsigned char*)malloc(200);  //申请200个字节的空间。 STM32空间动态申请存在较大的BUG，尽量别用，宁愿堆栈用多点
	volatile unsigned char prt_buffer[256];
	for(format_s_index = 0; format_s_index < cal_str_lth(format); format_s_index++) 
	{ 
		if(format[format_s_index] == '%'){ 
			para_type[para_num] = format[format_s_index+1]; 
			para_num++; 
		}else{ 
		} 
	}             
	va_start(arg_ptr,format); 
	format_s_index = 0; 
	prt_buffer_index = 0; 
	 
	for(para_index = 0; para_index < para_num; para_index++) 
	{ 
		unsigned char* deal_str = NULL; 
		unsigned char char_buf; 
		signed char schar_buf; 
		unsigned short short_buf; 
		signed short sshort_buf; 
		unsigned int uint_buf; 
		unsigned int sint_buf; 
		float float_buf; 
		unsigned char str_copy[256]; 
		unsigned char str_copy_index = 0; 
 
		//unsigned char format_limit; 
		//format_limit = cal_str_lth(format); 
 
		while((format[format_s_index] != '%')&&(prt_buffer_index<200)){ 
			prt_buffer[prt_buffer_index++] = format[format_s_index++]; 
		} 
		format_s_index += 2; //SKypt the %? 
		//need to make a "\r\n" in tail of every unsigned char[]? 
		prt_buffer[prt_buffer_index] = '\r'; 
		prt_buffer[prt_buffer_index+1] = '\n';	//How to makesure that the prt_buffer is not overflow 
		switch(para_type[para_index]) 
		{ 
			case 'c': 
				char_buf = va_arg(arg_ptr,unsigned char); 
				deal_str = uctochars(char_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 3; 
				break; 
			case 'C': 
				schar_buf = va_arg(arg_ptr,signed char); 
				deal_str = ctochars(schar_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 4; 
				break; 
			case 'h': 
				short_buf = va_arg(arg_ptr,unsigned short); 
				deal_str = uhwtochars(short_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 5; 
				break; 
			case 'H': 
				sshort_buf = va_arg(arg_ptr,signed short); 
				deal_str = shwtochars(sshort_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 6; 
				break; 
			case 'w': 
				uint_buf = va_arg(arg_ptr,unsigned int); 
				//bug_view_window = uint_buf; 
				deal_str = uwtochars(uint_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 10; 
				break; 
			case 'W': 
				sint_buf = va_arg(arg_ptr,signed int); 
				deal_str = swtochars(sint_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 11; 
				break; 
			case 'f': 
				float_buf =  va_arg(arg_ptr,float); 
				deal_str = ftochars(float_buf); 
				str_connect(prt_buffer,deal_str); 
				prt_buffer_index += 10; 
				break; 
			case 's': 
				deal_str = va_arg(arg_ptr,unsigned char*); 
				for(str_copy_index = 0; str_copy_index < cal_str_lth(deal_str); str_copy_index++) 
				{ 
					str_copy[str_copy_index] = deal_str[str_copy_index]; 
				} 
				str_connect(prt_buffer,str_copy);				 
				prt_buffer_index += (cal_str_lth(str_copy)); 
				break; 
			default: 
				break; 
		} 
		free(deal_str); 
		/* 
		for( ;format_s_index < format_limit; format_s_index++) 
		{ 
			prt_buffer[prt_buffer_index++] = format[format_s_index]; 
		} 
		*/ 
	} 
	if(para_num == 0){ 
		u1sPush(format,cal_str_lth(format)); 
	}else{ 
		u1sPush(prt_buffer,cal_str_lth(prt_buffer)); 
		//free(prt_buffer);	//release buffer space 
	} 
} 
 
unsigned char* u1rSlove(void) 
{ 
	//consider about such problem,when exc this function if enough int occurs ank make another "\r\n"! 
	static unsigned char slove_buf; 
	static unsigned char first_one = 1; 
	unsigned char* slove_str; 
	slove_str = NULL; 
	if(u1rGindex != u1rIndex){ 
		if((slove_buf == '\r')&&(u1rBuf[u1rGindex] == '\n')) 
		{ 
			//go on finding the previous "\r\n" 
			unsigned char back_index = u1rGindex; 
			unsigned char back_buf; 
			 
			back_index -= 2;	//if underflow, it will underflow itself! 
			 
			slove_str = (unsigned char*)calloc(256,sizeof(unsigned char)); 
			//slove_str = (unsigned char*)malloc(256);
			back_buf = u1rBuf[back_index]; 
			back_index--; 
			if(first_one == 1){ //description of the first time receive a command in effect! 
				unsigned char copy_index; 
				first_one = 0; 
				for(copy_index = 0; copy_index < u1rGindex; copy_index++) 
				{ 
					slove_str[copy_index] = u1rBuf[copy_index];	//copy the string with "\r\n"'s tail 
				} 
				//copy_index++; 
				slove_str[copy_index] = '\n'; 
			}else{ 
				unsigned char limit_count = 0; 
				while(1) 
				{	//1.we should add a limit,if the sense break the limit,return a unvalid result 
					if((u1rBuf[back_index] == '\r')&&(back_buf == '\n')) 
					{ 
						unsigned char copy_index = 0; 
						back_index +=2; 
						while(back_index != u1rGindex) 
						{ 
							slove_str[copy_index++] = u1rBuf[back_index]; 
							back_index++; 
						} 
						slove_str[copy_index] = '\n'; 
						break; 
					}else{ 
						limit_count++; 
						back_buf = u1rBuf[back_index]; 
						back_index--; 
						if(limit_count >= 100){ 
							break; 
						} 
					} 
				} 
			} 
		} 
		slove_buf = u1rBuf[u1rGindex]; 
		u1rGindex++; 
	} 
	return slove_str; 
} 
 
static unsigned char strCompare(unsigned char* str1,unsigned char* str2,unsigned char lth) 
{ 
	unsigned char sc_index; 
	for(sc_index = 0; sc_index < lth; sc_index++){ 
		if(str1[sc_index] == str2[sc_index]){ 
			 
		}else{ 
			return 1; 
		} 
	} 
	return 0; 
} 


unsigned char testArray[10];
unsigned char testArray2[10];
void sendHex(unsigned char *sData,unsigned char lth);

const unsigned char* testStr = "Hello kitty!";

unsigned char turnLevelFlag = 0;	//=1 Turn all High\ =2 Turn all Low
unsigned char turnLevelCnt = 0;		//最多切换33个

unsigned char ins700MsFlag = 0;
unsigned short ins700MsCnt = 0;


#include "timer.h"

#include "led.h"
unsigned char u1SprintBuf[50];
unsigned int msPreGet = 0;
unsigned int msAftGet = 0;
void dealUart1(unsigned char* buf){ 
	if(strCompare(buf,"TEST",4)==0){ 
		INFO1("UART1 Received TEST command!\r\n"); 
	}else if(strCompare(buf,"T485",4)==0){
		INFO1("T485 received!\r\n");
	}else if(strCompare(buf,"HOLD",4)==0){	//For test watch-Dog!
		INFO1("HOLD mode entered!\r\n");
		while(1);
	}else if(strCompare(buf,"TCRC",4)==0){
		unsigned short crcResult = 0;
		unsigned char buf[15];
		buf[0] = 0x5A;
		buf[1] = 0xA5;
		buf[2] = 0x04;
		buf[3] = 0x80;
		buf[4] = 0x03;
		buf[5] = 0x00;
		buf[6] = 0x00;
//		buf[7] = 0x00;
//		buf[8] = 0x02;
//		buf[9] = 0x01;
//		buf[10] = 0x00;
		INFO1("TCRC received!\r\n");
	}
	//INFO1("TheStr:%s\r\n",sBuf);
}
 
// 
#ifdef UART_1_TO_3 
void USART1_IRQHandler(void) 
{ 
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET) 
	{ 
		unsigned char temp; 
		temp = USART1->DR; 
		u1Receive(temp); 
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
	} 
	if(USART_GetITStatus(USART1,USART_IT_TC) != RESET) 
	{ 
 		u1Send(); 
	} 
} 
#endif //def UART_1_TO_3 
 
//#ifndef UART_1_TO_3 
// 
//void UART1_IRQHandler(void) 
//{ 
//	if(USART_GetITStatus(UART1,USART_IT_RXNE) != RESET) 
//	{ 
//		unsigned char temp; 
//		temp = UART1->DR; 
//		u1Receive(temp); 
//		USART_ClearITPendingBit(UART1, USART_IT_RXNE); 
//	} 
//	if(USART_GetITStatus(UART1,USART_IT_TC) != RESET) 
//	{ 
// 		u1Send(); 
//	} 
//} 
//#endif //undef UART_1_TO_3 
//EXTENDED_FUNCTION_TAIL


