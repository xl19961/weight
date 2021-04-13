#include "sys.h"
#include "usart_op.h"
#include <string.h>
#include <stdlib.h>
#include "master.h"
#include "relay.h"
#include "ad7191.h"
#include "usart_op.h"
uint8_t Rx_Deal_Str[(uint16_t)(UART_RX_BUFF_LEN*0.9)];
float value;
float  origin;
unsigned short CommIndex_Star = 0,UART_RecLen = 0;
void getStr(unsigned char *pData,unsigned char offset ,unsigned char len)
{
	unsigned char cnt;
	for(cnt = 0; cnt < len; cnt ++){
		Rx_Deal_Str[cnt] = pData[offset++];
//		if(offset >= UART_RX_BUFF_LEN) offset = 0;
	}
}
void str_copy(unsigned char targ[], unsigned char source[], short len)
{
	short cnt = 0;
	for(cnt = 0; cnt < len; cnt ++){
		targ[cnt] = source[cnt];
	}
}

unsigned char commdIsRec(USART_BufDef *DataS)
{
	while(((*DataS).Index) != (*DataS).pushIndex){
		if(((*DataS).Buf[(*DataS).Index - 1] == '\r') && ((*DataS).Buf[(*DataS).Index] == '\n')){
			if(((int16_t)((*DataS).Index) - (*DataS).commdStart) < 0){
				UART_RecLen = (*DataS).Index + 256 - (*DataS).commdStart + 1;
			}else{
				UART_RecLen = (*DataS).Index - (*DataS).commdStart + 1;
			}
			getStr((*DataS).Buf,(*DataS).commdStart,UART_RecLen);
//			if(UART_RecLen != 6){
//				TxError_cnt1 ++;
//				
//			}
			(*DataS).Index ++;
//			if(UART_RxIndex >= UART_RX_BUFF_LEN) UART_RxIndex = 0;
			(*DataS).commdStart = (*DataS).Index;
			return 1;
		}
			(*DataS).Index ++;
			//if(UART_RxIndex >= UART_RX_BUFF_LEN) UART_RxIndex = 0;
	}
	return 0;
}

unsigned char commdIsRec_uart1(USART_BufDef *DataS)
{
	static int times = 0;
	while(((*DataS).Index) != (*DataS).pushIndex){
		if(((*DataS).Buf[(*DataS).Index - 1] == 0x5a) && ((*DataS).Buf[(*DataS).Index] == 0xa5)){
			if(((int16_t)((*DataS).pushIndex) - (*DataS).Index) < 0){
				UART_RecLen = (*DataS).pushIndex + 256 - (*DataS).Index + 1;
			}else{
				UART_RecLen = (*DataS).pushIndex - (*DataS).Index + 1;
			}
			if(UART_RecLen >= 11)
			{
				getStr((*DataS).Buf,(*DataS).Index - 1,11);
				(*DataS).Index ++;
				return 1;
			}else{
				times++;
				if(times >= 2000)
				{
					times = 0;
					(*DataS).Index = (*DataS).pushIndex;
				}
			}
			return 0;
		}
			(*DataS).Index ++;
			//if(UART_RxIndex >= UART_RX_BUFF_LEN) UART_RxIndex = 0;
	}
	return 0;
}

 
unsigned long int Char_To_Int(unsigned char *nums,unsigned char len)
{
	unsigned long int num = 0;
	int ten = 1,cnt;
	for(cnt = len; cnt > 0  ; cnt --){
		if(nums[cnt - 1] != ' '){
			num += (nums[cnt - 1] - 0x30) * ten;
			ten *=10;
		}else
			break;
		
	}
	return num;
}
unsigned long int HexChar_To_int(unsigned char str[],unsigned char len)
{
	unsigned char cnt = 0;
	unsigned long int temp = 0;
	for(cnt = 0; cnt < len; cnt ++){
		if(str[cnt] != ' '){
			if(str[cnt] > 96){
				temp += ((str[cnt] - 87) << ((len - cnt - 1)  * 4));
			}else if(str[cnt] > 64){
				temp += ((str[cnt] - 55) << ((len - cnt - 1) * 4));
			}else{
				temp += ((str[cnt] & 0x0f) << ((len - cnt - 1) * 4));
			}
		}
	}
	return temp;
}
//���������жϺ���
StrIsEquals	str_compare(unsigned char* str1,unsigned char* str2,unsigned char lth)
{
	unsigned char sc_index;
	for(sc_index = 0; sc_index < lth; sc_index++){
		if(str1[sc_index] == str2[sc_index]){
			
		}else{
			return STR_UNEQUALS;
		} 
	}
	return STR_EQUALS;
}
unsigned char command_1[5] = {0x5a,0xa5,0x06,0x83,0x10};//����/ֹͣ
unsigned char command_2[6] = {0x5a,0xa5,0x08,0x83,0x00,0x10};//Ƥ��ת������
unsigned char command_3[6] = {0x5a,0xa5,0x08,0x83,0x10,0x10};//Ƥ��ת������
unsigned char command_4[6] = {0x5a,0xa5,0x08,0x83,0x10,0x12};//Ƥ��ת������
unsigned char command_5[6] = {0x5a,0xa5,0x06,0x83,0x10,0x34};//����
enum state{state_on,state_off,state_ide}; 
enum speed {speed_0,speed_change};
int value_old = 0;
int flag_state=0;
unsigned char over_top = 0, below_bottom = 0;
//���ڽ���
void USART_RecDeal(USART_TypeDef *USARTxD)
{	
	static unsigned char cnt = 0;
	unsigned char cnt_bl = 0;
	if(USARTxD == USART2){
		if(UART_RecLen > 7){
			for(cnt = 0; cnt < UART_RecLen - 8; cnt ++){
				if(str_compare(&Rx_Deal_Str[cnt],"CMD=",4)==STR_EQUALS ){
				}

			}
		}
		
	}
	if(USARTxD == USART1){
		//��Ƶ������ֹͣ����״̬����
		if(str_compare(&Rx_Deal_Str[cnt],command_1,5)==STR_EQUALS )
			{
			static enum state state_now,state_old = state_ide;
					
				//enum state state_now,state_old = state_ide;
			if(Rx_Deal_Str[8] == 0x01)
			{
			  state_now = state_off;
				flag_state = 1;
			}else if (Rx_Deal_Str[8] == 0x02){

				state_now = state_on;
				flag_state = 2;
			}else if (Rx_Deal_Str[8] == 0x00){
				state_now = state_ide;
				flag_state = 3;
			}
			if(state_now != state_old)
			{
				switch(state_now)
				{
					case state_on:
						WRITE_CF(0x31);
						break;
					case state_off:
						WRITE_CF(0x30);
						break;
					case state_ide:
						break;
					default:
						break;
				}	
			}
			state_old = state_now;
		}	
	//��Ƶ��ת�ٴ���
  if(str_compare(&Rx_Deal_Str[cnt],command_2,6)==STR_EQUALS )
    {  
			static int value_current = 0;
			value_current = Rx_Deal_Str[10];
			if(value_current != value_old){
			  if(Rx_Deal_Str[10] != 0x00)
					{
						WRITE_CF_HZ(Rx_Deal_Str[10]);
            
					}
					else if(Rx_Deal_Str[10] == 0x00)
					{
					;
					}

				value_old = value_current ;
				}
			value_old = value_current;
		}
		
	//��ŷ���������
	if(flag_state == 2)	
	{
			if(str_compare(&Rx_Deal_Str[cnt],command_3,6)==STR_EQUALS )//����
			{ 
				value=filter_AD();
				if(((value>2)&&(value<((Rx_Deal_Str[9]<<8)|Rx_Deal_Str[10])))&&(PBin(14)==0))
				{ 
					
						over_top = 1;
					
					
				}
			}	
			
			if(str_compare(&Rx_Deal_Str[cnt],command_4,6)==STR_EQUALS )//����
			{ 
				value=filter_AD();
				if((value>((Rx_Deal_Str[9]<<8)|Rx_Deal_Str[10]))&&(PBin(14)==0))
				{ 
					below_bottom  = 1;
					
				}
			}				
	}
	
	//��̬���㴦��
	if(str_compare(&Rx_Deal_Str[cnt],command_5,6)==STR_EQUALS )
		{  if(Rx_Deal_Str[8]==0x01)
			 { 
				 origin = amp;
				 
			 }
		}
	
	
	
}
}



// 	else if(USARTxD == USART3){
// 		if(UART_RecLen > 7){
// 			for(cnt = 0; cnt < UART_RecLen - 8; cnt ++){
// 				
// 			}
// 		}
// 		USART1_Push(Rx_Deal_Str,UART_RecLen);
// 	}
// 	if(str_compare(Rx_Deal_Str,"SRTC",4)==STR_EQUALS){
// 		RTC_Set(Char_To_Int(&Rx_Deal_Str[5],4),Char_To_Int(&Rx_Deal_Str[10],2),Char_To_Int(&Rx_Deal_Str[13],2),\
// 						Char_To_Int(&Rx_Deal_Str[16],2),Char_To_Int(&Rx_Deal_Str[19],2),RTC_GetCounter()%60);
// 	}




