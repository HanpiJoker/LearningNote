#include <linux/module.h>                                                 
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
/*
 *目的:为保证只能有一个应用程序唤醒驱动程序
 *方法2:通过自定互斥锁,当多个应用程序调用时
 		使其他应用程序进入不可中断的睡眠状态
 *另加:阻塞与非阻塞操作
 */
static struct class *sixthdrv_class;
static struct class_device	*sixthdrv_class_dev;

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;

volatile unsigned long *gpgcon;
volatile unsigned long *gpgdat;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);


static volatile int ev_press = 0;

static struct fasync_struct *button_async;


struct pin_desc{
	unsigned int pin;
	unsigned int key_val;
};


static unsigned char key_val;

struct pin_desc pins_desc[4] = {
	{S3C2410_GPF0, 0x01},
	{S3C2410_GPF2, 0x02},
	{S3C2410_GPG3, 0x03},
	{S3C2410_GPG11, 0x04},
};
 
//static atomic_t canopen = ATOMIC_INIT(1);
static DECLARE_MUTEX(button_lock);          //定义互斥锁

static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	struct pin_desc *pindesc = (struct pin_desc *) dev_id;
	unsigned int pinval;

	pinval = s3c2410_gpio_getpin(pindesc->pin);
	
	if (pinval)
	{

		key_val = 0x80 | pindesc->key_val;

	}
	else
	{
	
		key_val = pindesc->key_val;
	}

	ev_press = 1;
	wake_up_interruptible(&button_waitq);

	kill_fasync(&button_async, SIGIO, POLL_IN);

	return IRQ_HANDLED;
}

static int sixth_drv_open(struct inode *inode, struct file *file)
{
	/*if (!atomic_dec_and_test(&canopen))
	{
		atomic_inc(&canopen);
		return -EBUSY;
	}*/

	if (file->f_flags & O_NONBLOCK)
	{
		if (down_trylock (&button_lock))
			return -EBUSY;
	}
	else
	{
		//获取信号量
		down(&button_lock);
	}
	
	request_irq(IRQ_EINT0,  buttons_irq, IRQT_BOTHEDGE, "S2", &pins_desc[0]);
	request_irq(IRQ_EINT2,  buttons_irq, IRQT_BOTHEDGE, "S3", &pins_desc[1]);
	request_irq(IRQ_EINT11, buttons_irq, IRQT_BOTHEDGE, "S4", &pins_desc[2]);
	request_irq(IRQ_EINT19, buttons_irq, IRQT_BOTHEDGE, "S5", &pins_desc[3]);

	return 0;
}

//static ssize_t sixth_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
//{
	//return 0;
//}

static unsigned sixth_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); // 不会立即休眠

	if (ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

ssize_t sixth_drv_read (struct file *file, char __user *buf, size_t size, loff_t *ppos)
{	
	if  (size != 1)
		return -EINVAL;

	if (file->f_flags & O_NONBLOCK)
	{
		if (!ev_press)
			return -EAGAIN;
	}
	else
	{
		wait_event_interruptible(button_waitq, ev_press);
	}
	copy_to_user(buf, &key_val, 1);
	ev_press = 0;

	return 1;
}

int sixth_drv_close(struct inode *inode, struct file *file)
{
	//atomic_inc(&canopen);

	free_irq (IRQ_EINT0,  &pins_desc[0]);
	free_irq (IRQ_EINT2,  &pins_desc[1]);
	free_irq (IRQ_EINT11, &pins_desc[2]);
	free_irq (IRQ_EINT19, &pins_desc[3]);

	up (&button_lock);
	
	return 0;
}

static int sixth_drv_fasync (int fd, struct file *filp, int on)
{
	printk ("driver: sixth_drv_fasync!\n");	
	return fasync_helper (fd, filp, on, &button_async);
}



static struct file_operations sixth_drv_fops = {
	.owner = THIS_MODULE,
	.open = sixth_drv_open,
	/*.write = sixth_drv_write,*/
	.read = sixth_drv_read,
	.release = sixth_drv_close,
	.poll = sixth_drv_poll,
	.fasync =	sixth_drv_fasync,
};

int major;
static int sixth_drv_init(void)
{
	major = register_chrdev (0, "sixth_drv", &sixth_drv_fops);

	sixthdrv_class = class_create(THIS_MODULE, "sixthdrv");
	sixthdrv_class_dev = class_device_create(sixthdrv_class, NULL, MKDEV(major, 0), NULL, "buttons"); /* /dev/buttons */
	
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;
	return 0;
}

void sixth_drv_exit(void)
{
	unregister_chrdev(major, "sixth_drv");
	class_device_unregister(sixthdrv_class_dev);
	class_destroy(sixthdrv_class);

	iounmap (gpfcon);
	iounmap (gpgcon);
}

module_init (sixth_drv_init);
module_exit (sixth_drv_exit);

MODULE_LICENSE ("GPL");
