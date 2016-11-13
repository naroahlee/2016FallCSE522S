/*
 * Sample kobject implementation
 *
 * Copyright (C) 2004-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2007 Novell Inc.
 *
 * Released under the GPL version 2 only.
 *
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/pid.h>
#include <linux/sched.h>

/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */

static int fam_reader;

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t fam_reader_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	printk(KERN_NOTICE "%s\n", __func__);
	return sprintf(buf, "%d\n", fam_reader);
}

static ssize_t fam_reader_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int ret;
	struct task_struct* pstTr;
	struct task_struct* pstTrTemp;
	struct pid* pstId;
	//struct list_head* pstPoi;

	// Get fam_reader
	printk(KERN_NOTICE "%s\n", __func__);
	ret = kstrtoint(buf, 10, &fam_reader);
	if (ret < 0)
		return ret;

	if(NULL == (pstId = find_vpid(fam_reader)))
	{
		printk(KERN_ALERT "pstId = NULL\n");
		return count;
	}

	if(NULL == (pstTr = pid_task(pstId, PIDTYPE_PID)))
	{
		printk(KERN_ALERT "pstTr = NULL\n");
		return count;
	}

	pstTrTemp = pstTr;
	printk(KERN_NOTICE "PID = %d, comm = %s\n", pstTrTemp->pid, pstTrTemp->comm);
	while(1 != pstTrTemp->pid && 0 != pstTrTemp->pid)
	{
		pstTrTemp = pstTrTemp->real_parent;
		printk(KERN_NOTICE "PID = %d, comm = %s\n", pstTrTemp->pid, pstTrTemp->comm);
		// Print Siblings 
		//pstPoi = pstTrTemp->tasks.next;
		//pstTrTemp = list_entry(pstPoi, struct task_struct, tasks);
	}

	return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute fam_reader_attribute =
	__ATTR(fam_reader, 0664, fam_reader_show, fam_reader_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
	&fam_reader_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval;

	/*
	 * Create a simple kobject with the name of "kobject_example",
	 * located under /sys/kernel/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	 */
	example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");
