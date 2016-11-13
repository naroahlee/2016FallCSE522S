#include <stdio.h>
#include <stdlib.h>
 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "bcm2837_gpio.h"

/* Map Physical Memory to Virtual Memory: /dev/mem */
int map_peripheral(bcm2837_peripheral *pstr)
{
	// Open /dev/mem
	if ((pstr->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
	   fprintf(stderr, "Open /dev/mem failed! Checking permissions!\n");
	   exit(-1);
	}
	
	pstr->map = mmap(NULL,
					 BLOCK_SIZE,
					 PROT_READ|PROT_WRITE,
					 MAP_SHARED,
					 pstr->mem_fd,		
					 pstr->addr_p);

	if (pstr->map == MAP_FAILED) 
	{
		 perror("mmap");
		 exit(-1);
	}

	pstr->pstrReg = (gpio_regs *)pstr->map;

	return 0;
}
 
void unmap_peripheral(bcm2837_peripheral *pstr)
{ 
	munmap(pstr->map, BLOCK_SIZE);
	close(pstr->mem_fd);
}
 

/* Fxn Select: Refer to GPFSELn Registers */
int set_gpio_mode(bcm2837_peripheral* pstr, int s32ioid, int s32mode)
{
	gpio_regs* pstrReg;
	pstrReg = pstr->pstrReg;
	/* Clear */
	pstrReg->as32GPFSEL[s32ioid / 10] &= ~(0x7     << ((s32ioid % 10) * 3));
	/* Write 3 bit into Reg */
	pstrReg->as32GPFSEL[s32ioid / 10] |=  (s32mode << ((s32ioid % 10) * 3));
	return 0;
}

int set_gpio(bcm2837_peripheral* pstr, int s32ioid, int s32val)
{
	gpio_regs* pstrReg;
	pstrReg = pstr->pstrReg;

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

int get_gpio(bcm2837_peripheral* pstr, int s32ioid)
{
	gpio_regs* pstrReg;
	pstrReg = pstr->pstrReg;
	return ((pstrReg->as32GPLV[s32ioid / 32] >> (s32ioid % 32)) & 0x1);
}
