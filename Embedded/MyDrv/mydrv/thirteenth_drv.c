/*
 * 用内存模拟硬盘, 熟悉块设备驱动框架
 * 参考 \drivers\block\xd.c; \drivers\block\z2ram.c
*/
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>
	
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>

#define RAM2BLOCK_SIZE (1024 * 1024)
static unsigned char *ram2block_buf;

static struct gendisk *ram2block_disk;
static request_queue_t *ram2block_queue;

static DEFINE_SPINLOCK(ram2block_lock);

static int major;

static int ram2block_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	/* 容量 = heads * cylinders * sectors * 512 */
	geo->heads = 2;
	geo->sectors = 32;
	geo->cylinders = RAM2BLOCK_SIZE / 2 / 32 / 512;
	return 0;
}

static struct block_device_operations ram2block_fops = {
	.owner	= THIS_MODULE,
	.getgeo = ram2block_getgeo, 
};

static void do_ram2block_request (request_queue_t *q)
{
	//static int r_cnt = 0;
	//static int w_cnt = 0;
	struct request * req; 
	
//	printk ("do_ram2block_request %d \n", ++cnt);
	
	while ((req = elv_next_request(q)) != NULL)
	{
		/* 数据传输三要素:  源, 目的, 长度 */
		/* 源/目的    */
		unsigned long offset = req->sector *512 ;

		/* 目的/源 */
		
		
		/* 长度 */
		unsigned long len  = req->current_nr_sectors * 512;

		if (rq_data_dir(req) == READ)
		{	
			//printk ("do_ram2block_request read %d \n", ++r_cnt);
			memcpy(req->buffer, ram2block_buf + offset, len);
		}
		else
		{
		
			//printk ("do_ram2block_request write %d \n", ++w_cnt);
			memcpy(ram2block_buf + offset, req->buffer, len);
		}
		end_request(req, 1);
	} 
}

static int ram2block_init(void)
{
	/* 1. 分配一个gendisk结构体 */
	ram2block_disk = alloc_disk(16);  /* 次设备号个数:    分区个数       */
	if (!ram2block_disk)
			return -ENOMEM;
	/* 2. 设置 */
	/* 2.1 分配/设置队列:   提供读写能力 */
	ram2block_queue = blk_init_queue(do_ram2block_request, &ram2block_lock);
	ram2block_disk->queue = ram2block_queue;

	/* 2.2 设置其他属性: 比如容量 */
	major = register_blkdev(0, "ram2block");
	
	ram2block_disk->major = major;
	ram2block_disk->first_minor = 0;
	sprintf(ram2block_disk->disk_name, "ram2block");
	ram2block_disk->fops = &ram2block_fops;
	set_capacity(ram2block_disk, RAM2BLOCK_SIZE / 512);   /* 单位为扇区 */

	/* 3. 硬件相关操作 */
	ram2block_buf = kzalloc(RAM2BLOCK_SIZE, GFP_KERNEL);
	if (!ram2block_buf)
			return -ENOMEM;
	

	/* 4. 注册 */
    add_disk(ram2block_disk);

	return 0;
}

static void ram2block_exit(void)
{
    unregister_blkdev(major, "ram2block");
	del_gendisk(ram2block_disk);
	put_disk(ram2block_disk);
    blk_cleanup_queue(ram2block_queue);

	kfree(ram2block_buf);
} 

module_init(ram2block_init);
module_exit(ram2block_exit);
MODULE_LICENSE("GPL");

