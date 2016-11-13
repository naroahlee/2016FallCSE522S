#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

const char gacMyktname[] = "NaroahLab01";
int gs32data;
struct task_struct *gpkthmykth;
#define SEC2NSEC(x) (1000000000*(x))

static unsigned long log_sec  = 1;
static unsigned long log_nsec = 0;
static struct hrtimer gstmyhrt;

enum hrtimer_restart mycallback(struct hrtimer *psthrt)
{
	ktime_t s64current, s64interval;
	/* Reset Timer */
	s64current  = ktime_get();
	s64interval = ktime_set(log_sec, log_nsec);
	hrtimer_forward(psthrt, s64current, s64interval);

	/* Wake up my kthread */
	wake_up_process(gpkthmykth);

	return HRTIMER_RESTART;
}

int mykthread(void* ptr)
{
	int s32ret;
	ktime_t s64firstblood;

	printk(KERN_ALERT "Running my kernel thread Name = %s.\n", gacMyktname);

	/* Init Timer */
	s64firstblood = ktime_set(log_sec, log_nsec);	
	hrtimer_init(&gstmyhrt, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	gstmyhrt.function = &mycallback;
	hrtimer_start(&gstmyhrt, s64firstblood, HRTIMER_MODE_REL);

	/* Start Loop */
	while(!kthread_should_stop())
	{
		schedule();
		printk(KERN_NOTICE "nvcsw = %lu, nivcsw = %lu\n", current->nvcsw, current->nivcsw);
		//printk(KERN_NOTICE "I am in loop!\n");
		set_current_state(TASK_INTERRUPTIBLE);
	}
	/* When unload this module */
	printk(KERN_ALERT "Jump out from the inf loop!\n");

	/* Cancel Timer */
	s32ret = hrtimer_cancel(&gstmyhrt);
	if(1 == s32ret)
	{
		printk(KERN_ALERT "HR Timer is successfully canceled!\n");
	}
	printk(KERN_ALERT "HR Timer has been canceled!\n");

	return 0;
}

static int hello_init(void)
{
	printk(KERN_ALERT "Ready to start kthread.\n");
	printk(KERN_NOTICE "log_sec[%ld] log_nsec[%ld]\n",
		   log_sec,
		   log_nsec);
	gpkthmykth = kthread_run(mykthread, (void*)&gs32data, gacMyktname);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Unload module.\n");
	kthread_stop(gpkthmykth);
}

module_init(hello_init);
module_exit(hello_exit);
module_param(log_sec,  ulong, 0644);
module_param(log_nsec, ulong, 0644);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Haoran Li");
MODULE_DESCRIPTION("Kernel Monitoring Module");


