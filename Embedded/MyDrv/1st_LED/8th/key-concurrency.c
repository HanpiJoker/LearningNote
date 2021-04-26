/* 并态问题的解决(同步互斥阻塞) */
/* 1. 原子操作 */
/* 2. 信号量 */
/* 3. 阻塞与非阻塞 */

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/kernel.h>
#include<linux/delay.h>
#include<linux/irq.h> 
#include<asm/uaccess.h>
#include<asm/irq.h>
#include<asm/io.h>
#include<asm/arch/regs-gpio.h>
#include<asm/hardware.h>
#include<linux/poll.h>



#if 0
/* 原子操作 */
static atomic_t open = ATOMIC_INIT (1);
#endif

/* 信号量 */
static DECLARE_MUTEX(button_lock);

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);
static volatile int ev_press = 0;
static struct fasync_struct *button_async;

static int key_drv_major;
static struct class *key_drv_class;
static struct class_device *key_drv_class_dev;

struct cdev *key_drv_cdev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;
volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;


struct pin_desc    {
	unsigned int pin;
	int  key_val;
};

static unsigned char keyval[4] = {1, 1, 1, 1};
//static unsigned int keyval;
struct pin_desc pin_descs[4] = {
	{S3C2410_GPF0, 0},
	{S3C2410_GPF2, 1},
	{S3C2410_GPG3, 2},		
	{S3C2410_GPG11, 3},
};

static irqreturn_t buttons_irq (int irq, void *dev_id)
{
	struct pin_desc *pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval;

	pinval = s3c2410_gpio_getpin (pindesc->pin);

	if (pinval) {
		keyval[pindesc->key_val] = 1;
		//keyval = 0;
	} else {
		keyval[pindesc->key_val] = 0;
		//keyval = 1;
	}
	
	ev_press = 1;
	wake_up_interruptible (&button_waitq);
	kill_fasync (&button_async, SIGIO, POLL_IN);
	
	return IRQ_RETVAL (IRQ_HANDLED);
}
static int key_drv_open (struct inode *inode, struct file *filp)
{
#if 0
	/* 原子操作 */
	if (atomic_dec_and_test(&open) != 0)
	{
		open++;
		return -EBUSY;
	}
#endif

	if (filp->f_flags & O_NONBLOCK) {
		if (down_trylock(&button_lock))
			return -EBUSY;
	} else {
		/* 获取信号量 */
		down (&button_lock);
	}
	
	request_irq (IRQ_EINT0,  buttons_irq, IRQT_BOTHEDGE, "S2", &pin_descs[0]);
	request_irq (IRQ_EINT2,  buttons_irq, IRQT_BOTHEDGE, "S3", &pin_descs[1]);
	request_irq (IRQ_EINT11, buttons_irq, IRQT_BOTHEDGE, "S4", &pin_descs[2]);
	request_irq (IRQ_EINT19, buttons_irq, IRQT_BOTHEDGE, "S5", &pin_descs[3]);

	return 0;
}

static int key_drv_release (struct inode *inode, struct file *filp)
{
#if 0
	/* 原子操作 */
	atomic_inc (&open);
#endif

	/* 释放信号量 */
	up (&button_lock);

	free_irq (IRQ_EINT0, &pin_descs[0]);
	free_irq (IRQ_EINT2, &pin_descs[1]);
	free_irq (IRQ_EINT11, &pin_descs[2]);
	free_irq (IRQ_EINT19, &pin_descs[3]);

	key_drv_fasync (-1, filp, 0);
	
    return 0;
}


/*static ssize_t key_drv_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{

	return 0;
}*/

static ssize_t key_drv_read (struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{

    if (size != sizeof(keyval))
        return -EINVAL;
	
	if (filp->f_flags & O_NONBLOCK) {
		if (!ev_press)
			return -EAGAIN;
	} else {
		/* 如果没有按键的话就陷入休眠 */
		wait_event_interruptible (button_waitq, ev_press);
	}


    copy_to_user (buf, &keyval, sizeof(keyval));
	ev_press = 0;
	
    return sizeof(keyval);
}

static unsigned int key_drv_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int mask = 0;

	poll_wait(file, &button_waitq, wait);

	if (ev_press) {
		mask |= POLLIN | POLLRDNORM;
	}

	return mask;
}

static int key_drv_fasync(int fd, struct file *filp, int mode)
{
	return fasync_helper (fd, filp, mode, &button_async);
}

struct file_operations key_drv_fops =
{
    .owner = THIS_MODULE,
    .open = key_drv_open,
    //.write = key_drv_write,
    .read = key_drv_read,
    .release = key_drv_release,
    .poll = key_drv_poll,
    .fasync= key_drv_fasync,
};

static void key_drv_setup_cdev (struct cdev *cdev, int index)
{
    int err, devno = MKDEV (key_drv_major, index);

    cdev_init (cdev, &key_drv_fops);
    cdev->owner = THIS_MODULE;
    cdev->ops = &key_drv_fops;
    err = cdev_add(cdev, devno, 1);
    if (err)
        printk (KERN_NOTICE "Error %d adding globalmem %d", err, index);
}


static int __init key_drv_init (void)
{
    int result;
    dev_t devno = MKDEV(key_drv_major, 0);

    // 注册;分配主,次设备号
    if (key_drv_major)
        result = register_chrdev_region (devno, 1, "key");

    else
    {
        result = alloc_chrdev_region (&devno, 0, 1, "key");

        key_drv_major = MAJOR (devno);
    }
    if (result < 0)
    {
        printk (KERN_WARNING "key: can't get major %d\n", key_drv_major);
        return result;
    }
    key_drv_cdev = kzalloc (sizeof(struct cdev), GFP_KERNEL);
    if (!key_drv_cdev)
    {
        result = -ENOMEM;
        goto fail_malloc;
    }


    key_drv_setup_cdev (key_drv_cdev, 0);
    key_drv_class = class_create (THIS_MODULE, "key");
    if (IS_ERR(key_drv_class))
        return PTR_ERR (key_drv_class);
    key_drv_class_dev = class_device_create (key_drv_class, NULL, devno, NULL, "key1");
    if (unlikely (IS_ERR(key_drv_class_dev)))
        return PTR_ERR (key_drv_class_dev);

    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;
    gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
    gpgdat = gpgcon + 1;

    return 0;

fail_malloc:
    unregister_chrdev_region (devno, 1);
    return result;
}

static void __exit key_drv_exit (void)
{
    iounmap (gpfcon);
    iounmap (gpgcon);
    class_device_unregister (key_drv_class_dev);
    class_destroy (key_drv_class);
    cdev_del (key_drv_cdev);
    kfree (key_drv_cdev);
    unregister_chrdev_region (MKDEV(key_drv_major, 0), 1);
}

module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");

