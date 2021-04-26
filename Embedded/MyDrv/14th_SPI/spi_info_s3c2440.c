#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>

#include <linux/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

static struct spi_board_info spi_info_s3c2440[] = {
	{
		.modalias	= "spi_oled",
		.bus_num	= 1,
		.chip_select	= S3C2410_GPF(1),   // Ƭѡ
		.mode		= SPI_MODE_0,	/* CPOL=0, CPHA=0 */
		.max_speed_hz	= 10000000,
		.platform_data = (void *)S3C2410_GPG(4),
	},
	
	{
		.modalias	= "spi_flash",
		.bus_num	= 1,
		.chip_select	= S3C2410_GPG(2),   // Ƭѡ
		.mode		= SPI_MODE_0,	/* CPOL=0, CPHA=0 */
		.max_speed_hz	= 80000000,
	},
};



static int spi_info_s3c2440_init(void)
{
	return (spi_register_board_info(spi_info_s3c2440, ARRAY_SIZE(spi_info_s3c2440)));
}

module_init(spi_info_s3c2440_init);
MODULE_DESCRIPTION("OLED SPI Driver");
MODULE_AUTHOR("SpaceYu, 2694238260@qq.com");
MODULE_LICENSE("GPL");
