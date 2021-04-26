/* 参考:
 * ./drivers/mtd/devices/mtdram.c
 * ./drivers/mtd/devices/m25p80.c
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <sound/core.h>
#include <linux/spi/spi.h>
#include <asm/uaccess.h>
#include <linux/timer.h>

#include <mach/hardware.h>
#include <mach/regs-gpio.h>

#include <linux/gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/mtd/cfi.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>


static struct spi_device *spi_flash_dev;
void SPI_Flash_ReadID (int *pMID, int *pDID)
{
	unsigned char tx_buf[4] = {0x90, 0, 0, 0};
	unsigned char rx_buf[2];

	spi_write_then_read(spi_flash_dev, tx_buf, 4, rx_buf, 2);

	*pMID = rx_buf[0];
	*pDID = rx_buf[1];
	

}

static void SPI_Flash_Write_Enable(int enable)
{
	unsigned char val = enable ? 0x06 : 0x04;
	spi_write (spi_flash_dev, &val, 1);
}

static unsigned char SPI_Flash_Read_Status_Reg1(void)
{
	unsigned char val;
	unsigned char cmd = 0x05;

	spi_write_then_read(spi_flash_dev, &cmd, 1, &val, 1);

	return val;
}

static unsigned char SPI_Flash_Read_Status_Reg2(void)
{
	unsigned char val;
	unsigned char cmd = 0x35;

	spi_write_then_read(spi_flash_dev, &cmd, 1, &val, 1);

	return val;
}
static void SPI_Flash_Wait_Busy(void)
{
	/* BUSY  --- reg1 bit 0
	 * BUSY == 1    ---  busy
	 * BUSY == 0    ---  free
	 */
	while(SPI_Flash_Read_Status_Reg1() & 1)
	{    
        /* Sector erase time : 60ms
         * Page program time : 0.7ms
         * Write status reg time : 10ms
         *
		 * 休眠一段时间 (避免过度占用消耗CPU)
		 */
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout (HZ / 100);  /* 休眠10MS后再次判断 */
	};
}

static void SPI_Flash_Write_Status_Reg(unsigned char reg1, unsigned char reg2)
{
	unsigned char tx_buf[3];
	
	SPI_Flash_Write_Enable(1);

	tx_buf[0] = 0x01;
	tx_buf[1] = reg1;
	tx_buf[2] = reg2;
	
	spi_write (spi_flash_dev, tx_buf, 3);
	
	SPI_Flash_Wait_Busy();
}

static void SPI_Flash_Clean_Protect_Regs(void)
{
	unsigned char reg1, reg2;
	
	reg1 = SPI_Flash_Read_Status_Reg1();
	reg2 = SPI_Flash_Read_Status_Reg2();

	reg1 &= ~(1 << 7);	
	reg2 &= ~(1 << 0);

	SPI_Flash_Write_Status_Reg(reg1, reg2);
}

static void SPI_Flash_Clean_Protect_Data(void)
{
	/* cmp = 0, b2 ~ b0 = 0b000;
	 * cmp  ---  reg2 : bit 6
	 * b2 ~ b0   ---- reg1 : bit4 ~ bit2
	 */ 
	unsigned char reg1, reg2;
	
	reg1 = SPI_Flash_Read_Status_Reg1();
	reg2 = SPI_Flash_Read_Status_Reg2();

	// reg1 &= ~((1 << 4)|(1 << 3)|(1 << 2));	
	reg1 &= ~(7 << 2);
	reg2 &= ~(1 << 6);
 	
	SPI_Flash_Write_Status_Reg(reg1, reg2);
}

// erase 4K
void SPI_Flash_Erase_Sector(unsigned int address)
{
	unsigned char tx_buf[4];
	
	SPI_Flash_Write_Enable(1);

	tx_buf[0] = 0x20;
	tx_buf[1] = address >> 16;
	tx_buf[2] = address >> 8;
	tx_buf[3] = address & 0xff;

	spi_write (spi_flash_dev, tx_buf, 4);

	SPI_Flash_Wait_Busy();
}

// Program
void SPI_Flash_Program(unsigned int address, unsigned char *buf, int len)
{
	unsigned char tx_buf[4];
	/* 参考spi_write 代码实现 */
	struct spi_transfer	t[] = {
            {
    			.tx_buf		= tx_buf,
    			.len		= 4,
        	},
			{
    			.tx_buf		= buf,
    			.len		= len,
			},
		};
	struct spi_message	m;
	tx_buf[0] = 0x02;
    tx_buf[1] = address >> 16;
    tx_buf[2] = address >> 8;
    tx_buf[3] = address & 0xff;

	
	SPI_Flash_Write_Enable(1);
	
	spi_message_init(&m);
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);
	spi_sync(spi_flash_dev, &m);
	
	SPI_Flash_Wait_Busy();	
}

void SPI_Flash_Read(unsigned int address, unsigned char *buf, int len)
{
	/* spi_write_then_read规定了tx_cnt+rx_cnt < 32
     * 所以对于大量数据的读取，不能使用该函数
     */
	unsigned char tx_buf[4]; 
	struct spi_transfer	t[] = {
            {
    			.tx_buf		= tx_buf,
    			.len		= 4,
        	},
			{
    			.rx_buf		= buf,
    			.len		= len,
			},
		};
	struct spi_message	m;

	tx_buf[0] = 0x03;
    tx_buf[1] = address >> 16;
    tx_buf[2] = address >> 8;
    tx_buf[3] = address & 0xff;

	spi_message_init(&m);
	spi_message_add_tail(&t[0], &m);
	spi_message_add_tail(&t[1], &m);
	spi_sync(spi_flash_dev, &m);


}

static void SPI_Flash_Init(void)
{
	SPI_Flash_Clean_Protect_Regs();
	SPI_Flash_Clean_Protect_Data();

}

/* 构造注册一个mtd_info
 * mtd_device_register(master, parts, nr_parts)
 *
 */


/* 首先: 构造注册spi_driver
 * 然后: 在spi_driver的probe函数里构造注册mtd_info
 */
static struct mtd_info spi_flash_mtd;

static int spi_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	unsigned int address = instr->addr ;
	unsigned int len = 0;

    /* 判断参数 */
    if ((address & (spi_flash_mtd.erasesize - 1)) || (instr->len & (spi_flash_mtd.erasesize - 1)))
    {
        printk("addr/len is not aligned\n");
        return -EINVAL;
    }

	
	for (len  = 0 ; len < instr->len; len += 4096)
	{
		SPI_Flash_Erase_Sector(address);
        address += 4096;
	}
	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);
	return 0;
}

static int spi_flash_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	SPI_Flash_Read(from, buf, len);
	*retlen = len;
	return 0;
}

static int spi_flash_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
    unsigned int addr = to;
    unsigned int wlen  = 0;

    /* 判断参数 */
    if ((to & (spi_flash_mtd.erasesize - 1)) || (len & (spi_flash_mtd.erasesize - 1)))
    {
        printk("addr/len is not aligned\n");
        return -EINVAL;
    }

    for (wlen = 0; wlen < len; wlen += 256)
    {
        SPI_Flash_Program(addr, (unsigned char *)buf, 256);
        addr += 256;
        buf += 256;
    }

	*retlen = len;
	return 0;

}


static int __devinit spi_flash_probe(struct spi_device *spi)
{
    int mid, did;

	/* 构造注册一个mtd_info
     * mtd_device_register(master, parts, nr_parts)
     *
     */
    spi_flash_dev = spi; 

    s3c2410_gpio_cfgpin(spi->chip_select, S3C2410_GPIO_OUTPUT);
    SPI_Flash_Init();
    SPI_Flash_ReadID(&mid, &did);
    printk("SPI Flash ID: %02x %02x\n", mid, did);

	
	// memset(&spi_flash_mtd, 0, sizeof(spi_flash_mtd));

	/* Setup the MTD structure */
	spi_flash_mtd.name = "spi_flash";
	spi_flash_mtd.type = MTD_NORFLASH;
	spi_flash_mtd.flags = MTD_CAP_NORFLASH;
	spi_flash_mtd.size = 0x200000;      // 2M
	spi_flash_mtd.writesize = 1;
	spi_flash_mtd.writebufsize = 4096; /* 没什么卵用 */
	spi_flash_mtd.erasesize = 4096;

	spi_flash_mtd.owner = THIS_MODULE;
	spi_flash_mtd._erase = spi_flash_erase;
	spi_flash_mtd._read = spi_flash_read;
	spi_flash_mtd._write = spi_flash_write;

	if (mtd_device_register(&spi_flash_mtd, NULL, 0))
		return -EIO;

	return 0;
}

static int __devexit spi_flash_remove(struct spi_device *spi)
{
	mtd_device_unregister(&spi_flash_mtd);
	
	return 0;
}


static struct spi_driver spi_flash_driver = {
	.driver = {
		.name	= "spi_flash",
		.owner	= THIS_MODULE,
	},
	.probe		= spi_flash_probe,
	.remove		= __devexit_p(spi_flash_remove),
};


static int spi_flash_init(void)
{
	return spi_register_driver(&spi_flash_driver);
}

static void spi_flash_exit (void)
{
	spi_unregister_driver(&spi_flash_driver);
}


module_init(spi_flash_init);
module_exit(spi_flash_exit);
MODULE_DESCRIPTION("OLED SPI Driver");
MODULE_AUTHOR("SpaceYu, 2694238260@qq.com");
MODULE_LICENSE("GPL");

