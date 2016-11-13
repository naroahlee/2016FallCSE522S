#ifndef __MYGPIO_H__
#define __MYGPIO_H__

/* GPFSELn MODE Define */
#define GPIO_MODE_INPUT    (0)
#define GPIO_MODE_OUTPUT   (1)

typedef struct gpio_regs_t
{
	uint32_t as32GPFSEL[6];
	uint32_t as32Rsv1[1];
	uint32_t as32GPSET[2];
	uint32_t as32Rsv2[1];
	uint32_t as32GPCLR[2];
	uint32_t as32Rsv3[1];
	uint32_t as32GPLV[2];
} gpio_regs;

int set_gpio_mode(gpio_regs* pstrReg, int s32ioid, int s32mode);
int set_gpio(gpio_regs* pstrReg, int s32ioid, int s32val);
int get_gpio(gpio_regs* pstrReg, int s32ioid);

#endif
