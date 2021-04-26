#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <sound/core.h>
#include <linux/spi/spi.h>
#include <asm/uaccess.h>

#include <mach/hardware.h>
#include <mach/regs-gpio.h>

#include <linux/gpio.h>
#include <plat/gpio-cfg.h>

static int major;
static struct class *spi_cls;

static unsigned int spi_oled_dc_pin;
static unsigned char *spi_buf;
static struct spi_device *spi_oled_dev;

static void OLED_Set_DC(unsigned char val)
{
	s3c2410_gpio_setpin (spi_oled_dc_pin, val);
}

static void OLED_Write_Cmd(unsigned char cmd)
{
	OLED_Set_DC(0);   // Send Command

	spi_write(spi_oled_dev, &cmd, 1);

	OLED_Set_DC(1);   // Send
}

static void OLED_Write_Data(unsigned char data)
{
	OLED_Set_DC(1);   // Send data

	spi_write(spi_oled_dev, &data, 1);

	OLED_Set_DC(1);   // Send
}

static void OLED_Set_Page_Addr_Mode(void)
{
	OLED_Write_Cmd(0x20);
	OLED_Write_Cmd(0x02);
}

void OLED_Set_Pos(int page, int col)
{
	OLED_Write_Cmd (0xB0 + page);   // Set Page Start Address for Page Addressing Mode

	OLED_Write_Cmd (col & 0xf);     // Set Lower Column Start Address for Page Addressing Mode
	OLED_Write_Cmd (0x10 + (col >> 4));   // Set Higher Column Start Address for Page Addressing Mode
}

static void OLED_Clear(void)
{
	int page, i;
	for (page = 0; page < 8; page++)
	{
		OLED_Set_Pos (page, 0);
		for (i = 0; i < 128; i++)
			OLED_Write_Data (0);
	}
}

void OLED_Clear_Page(int page)
{
	int i;
	OLED_Set_Pos (page, 0);
	for (i = 0; i < 128; i++)
		OLED_Write_Data (0);
	
	OLED_Set_Pos (page + 1, 0);
	for (i = 0; i < 128; i++)
		OLED_Write_Data (0);

}

void OLED_Init(void)
{
	/* Send Command to Oled, Let it init */
	OLED_Write_Cmd (0xAE); /*display off*/
	OLED_Write_Cmd (0x00); /*set lower column address*/
	OLED_Write_Cmd (0x10); /*set higher column address*/
	OLED_Write_Cmd (0x40); /*set display start line*/
	OLED_Write_Cmd (0xB0); /*set page address*/
	OLED_Write_Cmd (0x81); /*contract control*/
	OLED_Write_Cmd (0x66); /*128*/
	OLED_Write_Cmd (0xA1); /*set segment remap*/
	OLED_Write_Cmd (0xA6); /*normal / reverse*/
	OLED_Write_Cmd (0xA8); /*multiplex ratio*/
	OLED_Write_Cmd (0x3F); /*duty = 1/64*/
	OLED_Write_Cmd (0xC8); /*Com scan direction*/
	OLED_Write_Cmd (0xD3); /*set display offset*/
	OLED_Write_Cmd (0x00);
	OLED_Write_Cmd (0xD5); /*set osc division*/
	OLED_Write_Cmd (0x80);
	OLED_Write_Cmd (0xD9); /*set pre-charge period*/
	OLED_Write_Cmd (0x1f);
	OLED_Write_Cmd (0xDA); /*set COM pins*/
	OLED_Write_Cmd (0x12);
	OLED_Write_Cmd (0xdb); /*set vcomh*/
	OLED_Write_Cmd (0x30);
	OLED_Write_Cmd (0x8d); /*set charge pump enable*/
	OLED_Write_Cmd (0x14);

	OLED_Set_Page_Addr_Mode();
	OLED_Clear();	

	OLED_Write_Cmd (0xAF); /*display ON*/
	

}

static ssize_t spi_oled_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	if (count > 4096)
		return -EINVAL;
	copy_from_user (spi_buf, buf, count);
	spi_write(spi_oled_dev, spi_buf, count);
	return 0;
}

#define OLED_CMD_INIT       0x100001
#define OLED_CMD_CLEAR_ALL  0x100002
#define OLED_CMD_CLEAR_PAGE 0x100003
#define OLED_CMD_SET_POS    0x100004

static long spi_oled_ioctl (struct file *file, unsigned int command, unsigned long arg)
{
	int page, col;
	
	switch (command)
	{
		case OLED_CMD_INIT:
		{
			OLED_Init();
			break;
		}
		
		case OLED_CMD_CLEAR_ALL:
		{
			OLED_Clear();
			break;
		}
		
		case OLED_CMD_CLEAR_PAGE:
		{
			page = arg;
			OLED_Clear_Page(page);
			break;
		}

		
		case OLED_CMD_SET_POS:
		{
			page = arg & 0xff;
			col  = (arg >> 8) & 0xff;
			OLED_Set_Pos(page, col);
			break;
		}
	}
	return 0;
}

static struct file_operations spi_oled_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = spi_oled_ioctl,
	.write          = spi_oled_write,
};

static int __devinit spi_oled_probe(struct spi_device *spi)
{
	spi_oled_dev = spi;
	spi_oled_dc_pin = (unsigned int)spi->dev.platform_data;
	s3c2410_gpio_cfgpin (spi_oled_dc_pin, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_cfgpin (spi->chip_select, S3C2410_GPIO_OUTPUT);

	if (!(spi_buf = kmalloc (4096, GFP_KERNEL)))
		return -ENOMEM;
	
	major = register_chrdev (0, "spi_oled", &spi_oled_fops);

	spi_cls = class_create (THIS_MODULE, "spi_oled");
	device_create (spi_cls, NULL, MKDEV(major, 0), NULL, "spi_oled");

	return 0;
}

static int __devexit spi_oled_remove(struct spi_device *spi)
{
	device_destroy (spi_cls, MKDEV(major, 0));
	class_destroy (spi_cls);
	unregister_chrdev (major, "spi_oled");
	kfree (spi_buf);
	return 0;
}

static struct spi_driver spi_oled_driver = {
	.driver = {
		.name	= "spi_oled",
		.owner	= THIS_MODULE,
	},
	.probe		= spi_oled_probe,
	.remove		= __devexit_p(spi_oled_remove),
};


static int spi_oled_init(void)
{
	return spi_register_driver(&spi_oled_driver);
}

static void spi_oled_exit (void)
{
	spi_unregister_driver(&spi_oled_driver);
}


module_init(spi_oled_init);
module_exit(spi_oled_exit);
MODULE_DESCRIPTION("OLED SPI Driver");
MODULE_AUTHOR("SpaceYu, 2694238260@qq.com");
MODULE_LICENSE("GPL");


