#ifndef __FLAG_DEAL_H_
#define __FLAG_DEAL_H_

void Flag_Deal(void);
void register_flag(void(*op)(void),unsigned char *flag);
void flag_sys_init(void);
#endif
