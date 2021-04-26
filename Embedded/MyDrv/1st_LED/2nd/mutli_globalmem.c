#include<linux/module.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define GLOBALMEM_SIZE 0x1000
#define MEM_CLEAR 0x1
#define GLOBALMEM_MAJOR 230
#define DEVICE_NUM 10

static int globalmem_major = GLOBALMEM_MAJOR;
module_param (globalmem_major, int, S_IRUGO);

static struct class *globalmem_class;
static struct class_device *globalmem_class_dev[DEVICE_NUM];

struct globalmem_dev {
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
};

struct globalmem_dev *globalmem_devp;

static int globalmem_open (struct inode *inode, struct file *filp)
{
	 
	struct globalmem_dev *dev = container_of (inode->i_cdev, struct globalmem_dev, cdev);
	filp->private_data = dev;

	return 0;
}

static int globalmem_release (struct inode *inode, struct file *filp)
{
	return 0;
}

static long globalmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;

	switch (cmd) {
	case MEM_CLEAR:
		memset (dev->mem, 0, GLOBALMEM_SIZE);
		printk (KERN_INFO "globalmem is set to zero\n");
		break;

	default :
		return -EINVAL;
	}
	return 0;
}
static ssize_t globalmem_read (struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned long count = size;
	int result = 0;
	struct globalmem_dev *dev = filp->private_data;

	if (p >= GLOBALMEM_SIZE)
		return 0;
	if (count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	if (copy_to_user (buf, dev->mem +p, count)) {
		result = -EFAULT;
	} else {
		*ppos += count;
		result = count;

		printk (KERN_INFO "read %u bytes from %lu\n", count, p);
	}

	return result;
}

static ssize_t globalmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int result = 0;
	struct globalmem_dev *dev = filp->private_data;
	
	if (p >= GLOBALMEM_SIZE)
		return 0;
	if (count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	if (copy_from_user (dev->mem + p, buf, count)) {
		result = -EFAULT;
	} else {
		*ppos += count;
		result = count;

		printk (KERN_INFO "written %u bytes from %lu\n", count, p);
	}

	return result;	
}

static loff_t globalmem_llseek (struct file *filp, loff_t offset, int orig)
{
	loff_t result = 0;

	switch (orig) {
	case 0:
		if (offset < 0) {
			result  = -EINVAL;
			break;
		}
		if ((unsigned int)offset > GLOBALMEM_SIZE) {
			result = -EINVAL;
			break;
		}
		filp->f_pos = (unsigned int)offset;
		result = filp->f_pos;
		break;
	case 1:
		if ((filp->f_pos + offset) > GLOBALMEM_SIZE) {
			result = -EINVAL;
			break;
		}
		if ((filp->f_pos + offset) < 0) {
			result = -EINVAL;
			break;
		}
		filp->f_pos += offset;
		result = filp->f_pos;
		break;
	default:
		result = -EINVAL;
		break;
	}
	return result;
}


struct file_operations globalmem_fops = {
	.owner = THIS_MODULE,
	.llseek = globalmem_llseek,
	.read = globalmem_read,
	.write = globalmem_write,
	.unlocked_ioctl = globalmem_ioctl,
	.open = globalmem_open,
	.release = globalmem_release,
};


static void globalmem_setup_cdev (struct globalmem_dev *dev, int index)
{
	int err, devno = MKDEV (globalmem_major, index);

	cdev_init (&dev->cdev, &globalmem_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &globalmem_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk (KERN_NOTICE "Error %d adding globalmem %d", err, index);
}
static int __init globalmem_init (void)
{
	int result;
	int i;
	dev_t devno = MKDEV(globalmem_major, 0);

	if (globalmem_major)
		result = register_chrdev_region (devno, DEVICE_NUM, "globalmem");
	else {
		result = alloc_chrdev_region (&devno, 0, DEVICE_NUM, "globalmem");
		globalmem_major = MAJOR (devno);
	}
	if (result < 0) {
		printk (KERN_WARNING "globalmem: can't get major %d\n", globalmem_major);
		return result;
	}
	globalmem_devp = kzalloc (sizeof(struct globalmem_dev), GFP_KERNEL);
	if (!globalmem_devp) {
		result = -ENOMEM;
		goto fail_malloc;
	}

	for (i = 0; i < DEVICE_NUM; i++)
		globalmem_setup_cdev (globalmem_devp + i, i);
	globalmem_class = class_create (THIS_MODULE, "globalmem");
	if (IS_ERR(globalmem_class))
		return PTR_ERR (globalmem_class);
	globalmem_class_dev[0] = class_device_create (globalmem_class, NULL, devno, NULL, "globalmem");
	for (i = 1; i < DEVICE_NUM; i++)
		globalmem_class_dev[i] = class_device_create (globalmem_class, NULL, MKDEV(globalmem_major, i), NULL, "globalmem%d", i);
		if (unlikely (IS_ERR(globalmem_class_dev[i])))
			return PTR_ERR (globalmem_class_dev[i]); 
		

	return 0;

	fail_malloc:
	unregister_chrdev_region (devno, 1);
	return result;
}

static void __exit globalmem_exit (void)
{
	int i;
	
	for (i = 0; i < DEVICE_NUM; i++)	
		class_device_unregister (globalmem_class_dev[i]);
	class_destroy (globalmem_class);
	for (i = 0; i < DEVICE_NUM; i++)
		cdev_del (&(globalmem_devp + i)->cdev);	
	kfree (globalmem_devp);
	unregister_chrdev_region (MKDEV(globalmem_major, 0), 1);
}

module_init(globalmem_init);
module_exit(globalmem_exit);
MODULE_LICENSE("GPL");

