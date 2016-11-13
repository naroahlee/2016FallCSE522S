/* bcm2837_gpio.h */
/* Header file for GPIO of RP3 (BCM2837) */

#ifndef __LED_HW_INC_H__
#define __LED_HW_INC_H__

#include <stdint.h>

#define BCM2837_PERI_BASE       0x3F000000
#define GPIO_BASE               (BCM2837_PERI_BASE + 0x200000) 
 
#define BLOCK_SIZE		(4*1024)

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
 
typedef struct bcm2837_peripheral_t
{
	uint32_t	addr_p;
	int32_t		mem_fd;
	void *		map;
	gpio_regs * pstrReg;
} bcm2837_peripheral;

int map_peripheral(bcm2837_peripheral *pstr);
void unmap_peripheral(bcm2837_peripheral *pstr);

int set_gpio_mode(bcm2837_peripheral* pstr, int s32ioid, int s32mode);
int set_gpio(bcm2837_peripheral* pstr, int s32ioid, int s32val);
int get_gpio(bcm2837_peripheral* pstr, int s32ioid);

#endif
