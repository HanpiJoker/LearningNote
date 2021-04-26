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
#include <linux/cdev.h>
#include <linux/dma-mapping.h>


#define MEM_COPY_NO_DMA   0
#define MEM_COPY_DMA      1
#define BUF_SIZE     (512 * 1024)

#define DMA0_BASE_ADDR 0x4B000000
#define DMA1_BASE_ADDR 0x4B000040
#define DMA2_BASE_ADDR 0x4B000080
#define DMA3_BASE_ADDR 0x4B0000C0

struct s3c_dma_regs {
	unsigned long disrc;
	unsigned long disrcc;
	unsigned long didst;
	unsigned long didstc;
	unsigned long dcon;
	unsigned long dstat;
	unsigned long dcsrc;
	unsigned long dcdst;
	unsigned long dmasktrig;
};

static volatile struct s3c_dma_regs *dma_regs;

static DECLARE_WAIT_QUEUE_HEAD(dma_waitq);
static volatile int ev_dma = 0;

static int major = 0;

static struct cdev s3c_dma_cdev; 
static struct class *cls;

static char *src;
static u32 src_phys;

static char *dst;
static u32 dst_phys;

static irqreturn_t s3c_dma_irq(int irq, void *devid)
{
	/* 唤醒 */
	ev_dma = 1;
    wake_up_interruptible(&dma_waitq);   
	return IRQ_HANDLED;
}

static int s3c_dma_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int i;

	memset (src, 0xAA, BUF_SIZE);
	memset (dst, 0xAA, BUF_SIZE);
	
	switch (cmd)
	{
		case MEM_COPY_NO_DMA :
		{
			for (i = 0; i < BUF_SIZE; i++)
				dst[i] = src[i];
			if (memcmp (src, dst, BUF_SIZE) == 0)
			{
				printk ("MEM_COPY_NO_DMA OK\n");
			}
			else
			{
				printk ("MEM_COPY_DMA ERROR\n");
			}
			break;
		}
		case MEM_COPY_DMA :
		{
			ev_dma = 0;
		
			/* 把源, 目的, 长度告诉DMA         */
		    dma_regs->disrc  = src_phys;        /* 源的物理地址 */
			dma_regs->disrcc = (0<<1) | (0<<0); /* 源位于AHB, 源地址递增 */
		    dma_regs->didst  = dst_phys;        /* 目的的物理地址 */
			dma_regs->didstc = (0<<1) | (0<<0); /* 目的位于AHB, 目的地址递增 */
			dma_regs->dcon   = (1<<30) | (1<<29) | (0<<28) | (1<<27) | (0<<23) | (0<<20) | (BUF_SIZE<<0); /* 使能中断, 单个传输, 软件触发 */

			/* 启动DMA */
			dma_regs->dmasktrig = (1<<1) | (1<<0);
			
			/* 如何知道DMA什么时候传输完成 */	
			/* 休眠 */	
			wait_event_interruptible(dma_waitq, ev_dma);

			if (memcmp(src, dst, BUF_SIZE) == 0)
			{
				printk("MEM_CPY_DMA OK\n");
			}
			else
			{
				printk("MEM_CPY_DMA ERROR\n");
			}

			break;
		}		
	}

	return 0;
}

static struct file_operations s3c_dma_fops = {

	.owner = THIS_MODULE,
	.ioctl  = s3c_dma_ioctl,
};


static int s3c_dma_init (void)
{
	dev_t devid;
	if(request_irq(IRQ_DMA3, s3c_dma_irq, 0, "s3c_dma", 1))
	{
		printk ("can't request _irq for DMA\n");
		return -EBUSY;
	}
	/* 分配src, dst对应的缓冲区         */
	src = dma_alloc_writecombine(NULL, BUF_SIZE, &src_phys, GFP_KERNEL);
	if (NULL == src)
	{
		printk ("can't alloc buffer for src\n");
		free_irq(IRQ_DMA3, 1);
		return -ENOMEM;
	}

	dst = dma_alloc_writecombine(NULL, BUF_SIZE, &dst_phys, GFP_KERNEL);
	if (NULL == dst)
	{
		dma_free_writecombine (NULL, BUF_SIZE, src, src_phys);
		free_irq(IRQ_DMA3, 1);
		printk ("can't alloc buffer for src\n");
		return -ENOMEM;
	}

	/* 注册 */	
	if (major) {
		devid = MKDEV(major, 0);
		register_chrdev_region(devid, 1, "s3c_dma");     /* (major, 0 ~ 1) 对应hello_fops, 其他的(major, 2) ~ (major, 255)都不对应hello_fops */
	} else {
		alloc_chrdev_region(&devid, 0, 1, "s3c_dma");     /* (major, 0 ~ 1) 对应hello_fops, 其他的(major, 2) ~ (major, 255)都不对应hello_fops */
		major = MAJOR(devid);
	}
		
	cdev_init(&s3c_dma_cdev, &s3c_dma_fops);
	cdev_add(&s3c_dma_cdev, devid, 1);

	/* 为了自动创建设备节点 */
	cls = class_create(THIS_MODULE, "s3c_dma");
	class_device_create(cls , NULL, MKDEV(major, 0), NULL, "s3c_dma0"); 
	
	dma_regs = ioremap (DMA3_BASE_ADDR, sizeof (struct s3c_dma_regs));		
	return 0;
}

static void s3c_dma_exit (void)
{
	class_device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	
	cdev_del(&s3c_dma_cdev);
	unregister_chrdev_region(MKDEV(major, 0), 1);

	dma_free_writecombine (NULL, BUF_SIZE, src, src_phys);
	dma_free_writecombine (NULL, BUF_SIZE, dst, dst_phys);

	free_irq(IRQ_DMA3, 1);

	iounmap(dma_regs);
}

module_init(s3c_dma_init);
module_exit(s3c_dma_exit);
MODULE_LICENSE ("GPL");

