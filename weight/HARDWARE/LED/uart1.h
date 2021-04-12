#ifndef    _UART1_H_
#define    _UART1_H_

//DEFINES_HEAD
//DEFINES_TAIL

//EXTERN_VAR_HEAD
//EXTERN_VAR_TAIL

//FUNCTION_CALL_HEAD
//FUNCTION_CAL_TAIL


extern unsigned int msPreGet;
extern unsigned int msAftGet;

extern unsigned char ins700MsFlag;
extern unsigned short ins700MsCnt;

void uart1Init(void);
void INFO1(unsigned char* format,...);
unsigned char* u1rSlove(void);
void dealUart1(unsigned char* buf);
void sendHex(unsigned char *sData,unsigned char lth);
void turnLevelTest(void);
#endif //_UART1_H_
