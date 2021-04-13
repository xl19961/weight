#include "FlagDeal.h"
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "ad7191.h"

struct flag{
	unsigned char *flag_list[64];
	void(*flag_op[64])(void);
	unsigned char flag_num;
}flag_deal;

void flag_sys_init(void)
{
	flag_deal.flag_num = 0;
}
void Flag_Deal(void)
{
	int i = 0;
	for (i = 0; i < flag_deal.flag_num; i++){
		if(*flag_deal.flag_list[i] == 1){
			flag_deal.flag_op[i]();
			*flag_deal.flag_list[i] = 0;
		}
	}
	if (PBin(14) == 0)
		light_sensor = 2;

}

void register_flag(void(*op)(void),unsigned char *flag)
{

	flag_deal.flag_op[flag_deal.flag_num] = op;
	flag_deal.flag_list[flag_deal.flag_num] = flag;
	flag_deal.flag_num++;
}


