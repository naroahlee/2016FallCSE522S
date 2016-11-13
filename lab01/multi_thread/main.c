#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/cpumask.h>

const char gacMyktname[] = "NaroLab01";
struct task_struct *gpkthmykth[4];

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
	wake_up_process(gpkthmykth[0]);
	wake_up_process(gpkthmykth[1]);
	wake_up_process(gpkthmykth[2]);
	wake_up_process(gpkthmykth[3]);

	return HRTIMER_RESTART;
}

int mykthread(void* ptr)
{
	printk(KERN_ALERT "Running my kernel thread Name = %s.\n", gacMyktname);

	/* Start Loop */
	while(!kthread_should_stop())
	{
		schedule();
		printk(KERN_NOTICE "nvcsw = %lu, nivcsw = %lu, @CPU[%d]\n",
				 current->nvcsw,
				 current->nivcsw,
				 smp_processor_id());
		set_current_state(TASK_INTERRUPTIBLE);
	}
	/* When unload this module */
	printk(KERN_ALERT "Jump out from the inf loop!\n");

	return 0;
}

static int hello_init(void)
{
	ktime_t s64firstblood;
	/* Init Timer */
	s64firstblood = ktime_set(log_sec, log_nsec);	
	hrtimer_init(&gstmyhrt, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	gstmyhrt.function = &mycallback;
	hrtimer_start(&gstmyhrt, s64firstblood, HRTIMER_MODE_REL);

	printk(KERN_ALERT "Ready to start kthread.\n");
	gpkthmykth[0] = kthread_create(mykthread, NULL, gacMyktname);
	gpkthmykth[1] = kthread_create(mykthread, NULL, gacMyktname);
	gpkthmykth[2] = kthread_create(mykthread, NULL, gacMyktname);
	gpkthmykth[3] = kthread_create(mykthread, NULL, gacMyktname);

	kthread_bind(gpkthmykth[0], 0);
	kthread_bind(gpkthmykth[1], 1);
	kthread_bind(gpkthmykth[2], 2);
	kthread_bind(gpkthmykth[3], 3);

	wake_up_process(gpkthmykth[0]);
	wake_up_process(gpkthmykth[1]);
	wake_up_process(gpkthmykth[2]);
	wake_up_process(gpkthmykth[3]);
	return 0;
}

static void hello_exit(void)
{
	int s32ret;

	printk(KERN_ALERT "HR Timer has been canceled!\n");
	printk(KERN_ALERT "Unload module.\n");
	kthread_stop(gpkthmykth[0]);
	kthread_stop(gpkthmykth[1]);
	kthread_stop(gpkthmykth[2]);
	kthread_stop(gpkthmykth[3]);

	/* Cancel Timer */
	s32ret = hrtimer_cancel(&gstmyhrt);
	if(1 == s32ret)
	{
		printk(KERN_ALERT "HR Timer is successfully Canceled!\n");
	}
}

module_init(hello_init);
module_exit(hello_exit);
module_param(log_sec,  ulong, 0644);
module_param(log_nsec, ulong, 0644);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shakespeare");
MODULE_DESCRIPTION("A Hello, World Module");


