/* 查询方式按键驱动LED */

#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/kernel.h>
#include<linux/delay.h>
#include<asm/uaccess.h>
#include<asm/irq.h>
#include<asm/io.h>
#include<asm/arch/regs-gpio.h>
#include<asm/hardware.h>


/* IO操作的宏定义 */
#define GPF_MSK(i)  (3<<(i*2))
#define GPG_MSK(i)  (3<<(i*2))
#define GPF_IN(i)   (0<<(i*2))
#define GPG_IN(i)   (0<<(i*2))
#define KEY_ON(i)  (1<<i)
#define LED_ON(i)  (1<<i)

static int key_drv_major;
static struct class *key_drv_class;
static struct class_device *key_drv_class_dev;

struct cdev *key_drv_cdev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;
volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;

static void delay_ms (unsigned char xms)
{
    int x, y;
    for (x=xms; x>0; x--)
        for (y=110; y>0; y--);
}

static int key_drv_open (struct inode *inode, struct file *filp)
{
    *gpfcon &= ~(GPF_MSK(0)|GPF_MSK(2));
    *gpfcon |= GPF_IN(0) | GPF_IN(2);

    *gpgcon &= ~(GPG_MSK(3)|GPG_MSK(11));
    *gpgcon |= GPG_IN(3) | GPG_IN(11);

    return 0;
}

static int key_drv_release (struct inode *inode, struct file *filp)
{
    return 0;
}


/*static ssize_t key_drv_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{

	return 0;
}*/

static ssize_t key_drv_read (struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned char key_vals[4];

    if (size != sizeof(key_vals))
        return -EINVAL;

    key_vals[0] = *gpfdat & KEY_ON(0) ? 1 : 0;
    key_vals[1] = *gpfdat & KEY_ON(2) ? 1 : 0;
    key_vals[2] = *gpgdat & KEY_ON(3) ? 1 : 0;
    key_vals[3] = *gpgdat & KEY_ON(11) ? 1 : 0;


    copy_to_user (buf, key_vals, sizeof(key_vals));

    return sizeof(key_vals);
}

struct file_operations key_drv_fops =
{
    .owner = THIS_MODULE,
    .open = key_drv_open,
    //.write = key_drv_write,
    .read = key_drv_read,
    .release = key_drv_release,
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

