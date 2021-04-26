/* 分配/设置/注册一个platform_driver */

#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>

static int major;

static struct class *cls;
static volatile unsigned long *gpfcon;
static volatile unsigned long *gpfdat;
static int pin;

static int led_open (struct inode *inode, struct file *filp)
{
    *gpfcon &= ~(0x3<<(pin * 2));
    *gpfcon |= (0x1<<(pin * 2));
	
    return 0;
}

static ssize_t led_write (struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	int val;

	copy_from_user (&val, buf, size);

	if (val) {
		*gpfdat &= ~(1<<pin);
	} else {
		*gpfdat |= (1<<pin);
	}
	
}


static struct file_operations led_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   led_open,     
	.write	=	led_write,	

};

static int led_probe(struct platform_device *pdev)
{	
	struct resource *res;
	
	res = platform_get_resource (pdev, IORESOURCE_MEM, 0);
	gpfcon = ioremap (res->start, res->end - res->start + 1);
	gpfdat = gpfcon + 1;

	res = platform_get_resource (pdev, IORESOURCE_IRQ, 0);
	pin = res->start;

	/* 注册字符设备驱动程序 */
	printk ("led_probe, found led\n");

	major = register_chrdev (0, "myled", &led_fops);

	cls = class_create (THIS_MODULE, "myled");
	class_device_create (cls, NULL, MKDEV(major, 0), NULL, "led");

	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	/* 卸载字符设备驱动程序 */
	/* iounmap */
	printk ("led_remove, remove led\n");

	class_device_destroy (cls, MKDEV(major, 0));
	class_destroy (cls);

	unregister_chrdev (major, "myled");
	iounmap(gpfcon);

	return 0;
}

struct platform_driver led_drv = {
	.probe		= led_probe,
	.remove		= led_remove,
	.driver		= {
		.name	= "myled",
	}
};

static int led_drv_init(void)
{
	platform_driver_register (&led_drv);
	return 0;
}
static void led_drv_exit(void)
{
	platform_driver_unregister (&led_drv);
}

module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_LICENSE("GPL");
