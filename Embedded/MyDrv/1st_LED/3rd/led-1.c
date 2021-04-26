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


#define GPF_OUT(i)  (1<<(i*2))
#define GPF_MSK(i)  (3<<(i*2))
#define LED_ON(i)  (1<<i)

static int led_drv_major;
static struct class *led_drv_class;
static struct class_device *led_drv_class_dev;

struct cdev *led_drv_cdev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

static int led_drv_open (struct inode *inode, struct file *filp)
{
    *gpfcon &= ~(GPF_MSK(4)|GPF_MSK(5)|GPF_MSK(6));
    *gpfcon |= GPF_OUT(4) | GPF_OUT(5) | GPF_OUT(6);
    return 0;
}

static int led_drv_release (struct inode *inode, struct file *filp)
{
    return 0;
}


static ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    unsigned char val[4];
    int i;

    copy_from_user(&val, buf, sizeof(val));

/*    if (1 == val) {
    	*gpfdat &= ~(1<<4);
    } else {
    	*gpfdat |= (1<<4);
    }
*/
    for (i=0; i<3; i++)
    {
        if (val[i] == 1)
            *gpfdat |= LED_ON(i+4);
        else
            *gpfdat &= ~ LED_ON(i+4);
    }
/*    if (val[0] == 1)
    	*gpfdat |= LED_ON(4);
    else
    	*gpfdat &= ~ LED_ON(4);

	
    if (val[1] == 1)
    	*gpfdat |= LED_ON(5);
    else
    	*gpfdat &= ~ LED_ON(5);

	
    if (val[2] == 1)
    	*gpfdat |= LED_ON(6);
    else
    	*gpfdat &= ~ LED_ON(6);
	

	if (val[3] == 1)
        *gpfdat |= LED_ON(4)|LED_ON(5)|LED_ON(6);
    else
        *gpfdat &= ~(LED_ON(4)|LED_ON(5)|LED_ON(6)); */

    return 0;
}

struct file_operations led_drv_fops =
{
    .owner = THIS_MODULE,
    .open = led_drv_open,
    .write = led_drv_write,
    .release = led_drv_release,
};

static void led_drv_setup_cdev (struct cdev *cdev, int index)
{
    int err, devno = MKDEV (led_drv_major, index);

    cdev_init (cdev, &led_drv_fops);
    cdev->owner = THIS_MODULE;
    cdev->ops = &led_drv_fops;
    err = cdev_add(cdev, devno, 1);
    if (err)
        printk (KERN_NOTICE "Error %d adding globalmem %d", err, index);
}


static int __init led_drv_init (void)
{
    int result;
    dev_t devno = MKDEV(led_drv_major, 0);

    // 注册;分配主,次设备号
    if (led_drv_major)
        result = register_chrdev_region (devno, 1, "led");

    else
    {
        result = alloc_chrdev_region (&devno, 0, 1, "led");

        led_drv_major = MAJOR (devno);
    }
    if (result < 0)
    {
        printk (KERN_WARNING "led: can't get major %d\n", led_drv_major);
        return result;
    }
    led_drv_cdev = kzalloc (sizeof(struct cdev), GFP_KERNEL);
    if (!led_drv_cdev)
    {
        result = -ENOMEM;
        goto fail_malloc;
    }


    led_drv_setup_cdev (led_drv_cdev, 0);
    led_drv_class = class_create (THIS_MODULE, "led");
    if (IS_ERR(led_drv_class))
        return PTR_ERR (led_drv_class);
    led_drv_class_dev = class_device_create (led_drv_class, NULL, devno, NULL, "led1");
    if (unlikely (IS_ERR(led_drv_class_dev)))
        return PTR_ERR (led_drv_class_dev);

    gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
    gpfdat = gpfcon + 1;

    return 0;

fail_malloc:
    unregister_chrdev_region (devno, 1);
    return result;
}

static void __exit led_drv_exit (void)
{
    iounmap (gpfcon);
    class_device_unregister (led_drv_class_dev);
    class_destroy (led_drv_class);
    cdev_del (led_drv_cdev);
    kfree (led_drv_cdev);
    unregister_chrdev_region (MKDEV(led_drv_major, 0), 1);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");

