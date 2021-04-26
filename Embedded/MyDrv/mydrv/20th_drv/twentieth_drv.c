/*
 * 定时器消抖
 * 字符设备驱动新写法
 */

#include <linux/module.h>                                                 
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

#define HELLO_CNT 2

/* 1. 确定主设备号 */
static int major;

static int hello_open (struct inode *inode, struct file *file)
{
	printk ("hello_open\n");
	return 0;
}

/* 2. 构造file_operations */
static struct file_operations hello_fops = {

	.owner = THIS_MODULE,
	.open  = hello_open,
};

static struct cdev hello_cdev; 

static struct class *cls;

static int hello_init (void)
{
	dev_t devid;
	
	/* 3. 告诉内核 */
#if 0
	major = register_chrdev (0, "hello", &hello_fops);
	/* 上述方法存在不足:
	 * (major, 0), (major, 1) ... (major, 255)均对应着hello_fops
	 * 浪费了太多次设备号
	 */
#else

	if (major) {
		devid = MKDEV(major, 0);
		register_chrdev_region(devid, HELLO_CNT, "hello_dev");     /* (major, 0 ~ 1) 对应hello_fops, 其他的(major, 2) ~ (major, 255)都不对应hello_fops */
	} else {
		alloc_chrdev_region(&devid, 0, HELLO_CNT, "hello_dev");     /* (major, 0 ~ 1) 对应hello_fops, 其他的(major, 2) ~ (major, 255)都不对应hello_fops */
		major = MAJOR(devid);
	}

	

	cdev_init(&hello_cdev, &hello_fops);
	cdev_add(&hello_cdev, devid, HELLO_CNT);
 
#endif


	cls = class_create(THIS_MODULE, "hello");
	class_device_create(cls , NULL, MKDEV(major, 0), NULL, "hello0"); /* /dev/buttons */	
	class_device_create(cls , NULL, MKDEV(major, 1), NULL, "hello1"); /* /dev/buttons */	
	class_device_create(cls , NULL, MKDEV(major, 2), NULL, "hello2"); /* /dev/buttons */	

	return 0;
}

static void hello_exit (void)
{
	class_device_destroy(cls, MKDEV(major, 0));
	class_device_destroy(cls, MKDEV(major, 1));
	class_device_destroy(cls, MKDEV(major, 2));
	class_destroy(cls);

	
	cdev_del(&hello_cdev);
	unregister_chrdev_region(MKDEV(major, 0), HELLO_CNT);
}

module_init (hello_init);
module_exit (hello_exit);
MODULE_LICENSE ("GPL");

