#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include "mygpio.h"
int set_gpio_mode(gpio_regs* pstrReg, int s32ioid, int s32mode)
{
	/* Clear */
	pstrReg->as32GPFSEL[s32ioid / 10] &= ~(    0x7 << ((s32ioid % 10) * 3));
	/* Write 3 bit into Reg */
	pstrReg->as32GPFSEL[s32ioid / 10] |=  (s32mode << ((s32ioid % 10) * 3));
	return 0;
}

int set_gpio(gpio_regs* pstrReg, int s32ioid, int s32val)
{
	if(s32val)
	{
		pstrReg->as32GPSET[s32ioid / 32] = (1 << (s32ioid % 32));
	}
	else
	{
		pstrReg->as32GPCLR[s32ioid / 32] = (1 << (s32ioid % 32));
	}
	return 0;
}

int get_gpio(gpio_regs* pstrReg, int s32ioid)
{
	return ((pstrReg->as32GPLV[s32ioid / 32] >> (s32ioid % 32)) & 0x1);
}
