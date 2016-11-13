#include <stdio.h>
#include <stdlib.h>
 
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "bcm2837_gpio.h"

#define LED_PIN (4)

bcm2837_peripheral gstrgpio =
{
	.addr_p = GPIO_BASE,
};
 

int main(int argc, char* argv[])
{
	int s32i = 0;
	volatile int s32temp = 0;

	if(0 != map_peripheral(&gstrgpio)) 
	{
		fprintf(stderr, "Mapping Fail.\n");
		exit(-1);
	}

	/* Set GPIO PIN as output */
	(void)set_gpio_mode(&gstrgpio, LED_PIN, GPIO_MODE_OUTPUT);

	for(s32i = 0; s32i < 10; s32i++)
	{
		set_gpio(&gstrgpio, LED_PIN, 1);
		s32temp = get_gpio(&gstrgpio, LED_PIN);
		printf("Set GPIO[%d] = %d\n", LED_PIN, s32temp);
		
		sleep(1);

		set_gpio(&gstrgpio, LED_PIN, 0);
		s32temp = get_gpio(&gstrgpio, LED_PIN);
		printf("Set GPIO[%d] = %d\n", LED_PIN, s32temp);
		sleep(1);
	}

	(void)set_gpio_mode(&gstrgpio, LED_PIN, GPIO_MODE_INPUT);
	unmap_peripheral(&gstrgpio);

	return 0;
}
