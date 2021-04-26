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

static struct class *led_drv_class;
static struct class_device	*led_drv_class_dev;

static volatile unsigned long *gpio_con;
static volatile unsigned long *gpio_dat;
static int pin;

static int major;

static int led_drv_open(struct inode *inode, struct file *file)
{
	/* 配置为输出引脚 */
	*gpio_con &= ~(0x3<<(pin * 2));
	*gpio_con |= (0x1<<(pin * 2));
	return 0;
}

static ssize_t led_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;
	copy_from_user(&val, buf, count); //	copy_to_user();
	
	if (val == 1)
	{
		/* 点灯 */
		*gpio_dat &= ~(1<< pin);
	}
	else
	{
		/* 关灯 */
		*gpio_dat |= (1<< pin);
	}
		
	return 0;

}

static struct file_operations led_fops = {
	.owner  =   THIS_MODULE,    
    .open   =   led_drv_open,     
	.write	=	led_drv_write,		

};

static int led_probe(struct platform_device *pdev)
{	
	struct resource		*res;
	
	/* 根据platform_device的资源进行ioremap */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	gpio_con = ioremap (res->start, res->end - res->start + 1);
	gpio_dat = gpio_con + 1;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	pin = res->start;
	
	/* 注册字符设备驱动 */
	major = register_chrdev(0, "my_led", &led_fops);

	led_drv_class = class_create(THIS_MODULE, "my_led");
	led_drv_class_dev = class_device_create(led_drv_class, NULL, MKDEV(major, 0), NULL, "led"); /* /dev/xyz */

	printk ("led_probe, found led\n");

	return 0;
}

static int led_remove(struct platform_device *pdev)
{
	unregister_chrdev(major, "my_led"); //

	/* 卸载字符设备驱动 */
	class_device_unregister(led_drv_class_dev);
	class_destroy(led_drv_class);
	
	/* iouremap */
	iounmap(gpio_con);

	return 0;
}


struct platform_driver led_drv = {
	.probe		= led_probe,
	.remove		= led_remove,
	.driver		= {
		.name	= "my_led",
	}
};


static int led_drv_init(void)
{
	return platform_driver_register(&led_drv);
}

static void led_drv_exit(void)
{
	platform_driver_unregister(&led_drv);
}

module_init(led_drv_init);
module_exit(led_drv_exit);


MODULE_LICENSE("GPL");

