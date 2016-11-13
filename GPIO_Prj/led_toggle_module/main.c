#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/err.h>
#include <linux/device.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <mach/platform.h>
#include "mygpio.h"

/* GPIO Physical PIN */
#define LED_PIN (4)

gpio_regs* gpstrReg = NULL;

/* Timer for toggling LED */
static struct timer_list gstrTimer;
static int gs32Period = 1000;

/* Global LED Status */
static int gs32isoff = 1;

/* Sysfs: Device Class Pointer */
static struct class  *pstrDevC;
static struct device *pstrDevO;

static void fxn_timer_hdl(unsigned long unused)
{
    gs32isoff = !gs32isoff;
	set_gpio(gpstrReg, LED_PIN, gs32isoff);
    mod_timer(&gstrTimer,
              jiffies + msecs_to_jiffies(gs32Period));
	return;
}

static ssize_t set_period(struct device* dev,
                          struct device_attribute* attr,
                          const char* buf,
                          size_t count)
{
	long s32val = 0;
	if (kstrtol(buf, 10, &s32val) < 0)
		return -EINVAL;
	/* Robustness: Frequency <= 100 Hz */
	if (s32val < 10)
		return -EINVAL;

	gs32Period = s32val;
	return count;
}

/* Using Marco to Define global dev_attr_period */
static DEVICE_ATTR(period,
                   0644,
                   NULL,
                   set_period);



/* ============= main: Module Initalization / Destroy ================ */

static int led_ctrl_init(void)
{
	/* Step0: Map the Physical Memory into Virtual Memory */
	gpstrReg  = (gpio_regs *)__io_address(GPIO_BASE);
	/* Step1: Set PIN as Output */
	gs32isoff = get_gpio(gpstrReg, LED_PIN);
	set_gpio_mode(gpstrReg, LED_PIN, GPIO_MODE_OUTPUT);

	/* Step2: Setup Timer */
	setup_timer(&gstrTimer, &fxn_timer_hdl, 0);
	mod_timer(&gstrTimer, jiffies + msecs_to_jiffies(gs32Period));

	/* Step3: Create Sysfs Hierarchy for setting attribution */

	/* Level 0: /sys/class/CustomLed */
	pstrDevC = class_create(THIS_MODULE, "CustomLed");
	/* Level 1: /sys/class/CustomLed/Led0 */
	pstrDevO = device_create(pstrDevC,
							NULL,
							0,
							NULL,
							"Led0");

	/* Level 2: Attribution /sys/class/CustomLed/Led0/period */
	device_create_file(pstrDevO, &dev_attr_period);

	printk(KERN_ALERT "Init LED Controller.\n");
	return 0;
}

static void led_ctrl_exit(void)
{
	/* Step1: Destroy Sysfs */
	device_remove_file(pstrDevO, &dev_attr_period);
	device_destroy(pstrDevC, 0);
	class_destroy(pstrDevC);
	/* Step2: Destroy Timer */
	del_timer(&gstrTimer);
	/* Step3: Set LED_PIN as Input */
	set_gpio_mode(gpstrReg, LED_PIN, GPIO_MODE_INPUT);

	printk(KERN_ALERT "Unload LED module.\n");
	return;
}

/* ============================================== */

module_init(led_ctrl_init);
module_exit(led_ctrl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naroahlee");
MODULE_DESCRIPTION("LED Toggle Module");
