/* LCD驱动程序 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>

static int s3c_lcdfb_setcolreg(unsigned regno,unsigned red, 
					unsigned green, unsigned blue,
			        unsigned transp, struct fb_info *info);

struct lcd_regs {
	unsigned long lcdcon1;
	unsigned long lcdcon2;
	unsigned long lcdcon3;
	unsigned long lcdcon4;
	unsigned long lcdcon5;
	unsigned long lcdsaddr1;
	unsigned long lcdsaddr2;
	unsigned long lcdsaddr3;
	unsigned long redlut;
	unsigned long greenlut;
	unsigned long bluelut;
	unsigned long reserved[9];
	unsigned long dithmode;
	unsigned long tpal;
	unsigned long lcdintpnd;
	unsigned long lcdsrcpnd;
	unsigned long lcdintmsk;
	unsigned long tconsel;
};

static struct fb_info *s3c_lcd;

static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;

static volatile struct lcd_regs *lcd_regs;
static u32 pseudo_palette[16];

static struct fb_ops s3c_lcdfb_ops = {
	.owner          = THIS_MODULE,
	.fb_setcolreg	= s3c_lcdfb_setcolreg,      /* 调色板 */
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}


static int s3c_lcdfb_setcolreg(unsigned regno,unsigned red, 
					unsigned green, unsigned blue,
			        unsigned transp, struct fb_info *info)
{
	unsigned int val;
	
	if(regno > 16)
		return 1;
	/* 用红, 绿, 蓝三原色构造出val */
	val  = chan_to_field(red,   &info->var.red);
	val |= chan_to_field(green, &info->var.green);
	val |= chan_to_field(blue,  &info->var.blue);
	((u32 *)(info->pseudo_palette))[regno] = val;
	return 0;

}

static int  lcd_init(void)
{
	/* 1. 分配一个fb_info */
	s3c_lcd = framebuffer_alloc(0, NULL);
	if (!s3c_lcd) {
		return -ENOMEM;
	}
	
	/* 2. 设置 */
	/* 2.1 设置固定参数 */
	strcpy(s3c_lcd->fix.id, "mylcd");
	s3c_lcd->fix.smem_len = 480 * 272 * 16 / 8;    // 5:6:5 RGB
	s3c_lcd->fix.type = FB_TYPE_PACKED_PIXELS;
	s3c_lcd->fix.visual = FB_VISUAL_TRUECOLOR;	/* TFTLCD是真彩色 */
	s3c_lcd->fix.line_length = 480 * 2;
	
	/* 2.2 设置可变参数 */
	s3c_lcd->var.xres           = 480;
	s3c_lcd->var.yres           = 272;
	s3c_lcd->var.xres_virtual   = 480;
	s3c_lcd->var.yres_virtual   = 272;
	s3c_lcd->var.bits_per_pixel = 16;

	/* RGB: 565 */
	s3c_lcd->var.red.offset     = 11;
	s3c_lcd->var.red.length     = 5;

	s3c_lcd->var.green.offset     = 6;
	s3c_lcd->var.green.length     = 6;

	s3c_lcd->var.blue.offset     = 0;
	s3c_lcd->var.blue.length     = 5;

	s3c_lcd->var.activate = FB_ACTIVATE_NOW;
	
	/* 2.3 设置操作函数 */
	s3c_lcd->fbops = &s3c_lcdfb_ops;
	
	/* 2.4 其他的设置 */
	s3c_lcd->pseudo_palette = pseudo_palette;
	s3c_lcd->screen_size    = 480 * 272 * 16 / 8;
	/* 3. 硬件相关操作 */
	/* 3.1 配置GPIO用与LCD */
	gpbcon = ioremap (0x56000010, 8);
	gpbdat = gpbcon + 1;
	gpccon = ioremap (0x56000020, 4);
	gpdcon = ioremap (0x56000030, 4);
	gpgcon = ioremap (0x56000060, 4);

	*gpccon = 0xaaaaaaaa;    /* GPIO管脚用于VD[7:0] LCDVF[2:0], VM, VFRAME, VLINE, VCLK, LEND */
	*gpdcon = 0xaaaaaaaa;    /* GPIO管脚用于VD[23:8] */
	*gpbcon &= ~(3);         /* GPBO设置为输出引脚 */
	*gpbcon |= 1;
	*gpbdat &= ~1;            /* 输出低电平 */
	
	*gpgcon |= (3<<8);       /* GPG4用作LCD_PWREN */
	
	/* 3.2 根据LCD手册设置LCD控制器，比如VCLK的频率 */
	lcd_regs = ioremap (0x4D000000, sizeof(struct lcd_regs));

	/* bit[17:8]: VCLK = HCLK / [(CLKVAL + 1) * 2];
	 *            10MHz = 100MHz / [(CLKVAL + 1) * 2];
	 *            CLKVAL = 4;
	 * bit[6:5] : 0b11 TFTLCD;
	 * bit[4:1] : 0b1100 16bpp TFTLCD;
	 * bit[0]   : 0 = Disabled video output and the LCD control signal; 
	 */
	lcd_regs->lcdcon1 = ((4<<8) | (3<<5) | (0x0C<<1));

	/* 垂直方向的时间参数
	 * bit[31:24]: VBPD = 3, VSYNC之后再过多长时间才能发出第一行数据, 
	 *                       VBPD = T0 - T2 - T1 - 1 = 3;
	 * bit[23:14] : LINEVAL = 639, 多少行, LINEVAL = 480 - 1 = 639;
	 * bit[13:6] : VFPD = 1, 发出最后一行数据之后，再过多长时间才发出VSYNC信号,
	 *						  VFPD = T2 - T5 - 1 = 1;
	 * bit[5:0]   : VSPW = 0, VSYNC信号的脉冲宽度，
	 *						  VSPW = T1 - 1; 
	 */
	lcd_regs->lcdcon2 = (3<<24) | (271<<14) | (1<<6) | (0<<0);
	
	/* 水平的时间参数
	 * lcdcon3
	 * bit[25:19]: HBPD = 16, HSYNC之后过多长时间才能发出第一行数据，
	 *                        HBPD = T6 - T9 - 1 = 16;
	 * bit[18：8] : HOZVAL = 479, 多少列，272，HOZVAL = 272 - 1;
	 * bit[7：0] : HFPD = 10, 发出最后一行里最后一个象素数据之后, 再过多久才发出HSYNC信号，
	 *                        HFPD = T8 - T11 - 1 = 10;
	 * lcdcon4
	 * bit[15:8] : (另外一款LCD的参数，忽略)
	 * bit[7:0] : HSPW = 4, HSYNC信号的脉冲宽度，HSPW = T7 - 1； 
	 */
	lcd_regs->lcdcon3 = (16<<19) | (479<<8) | (10<<0);
	lcd_regs->lcdcon4 = (4<<0);

	/* 信号的极性lcdcon5 (initable 0)
	 * bit[11] : 1, FRM565    : 5:6:5 Format;
	 * bit[10] : 0, INVVCLK   : The video data is fetched at VCLK falling edge;
	 * bit[9]  : 1, INVVLINE  : HSYNC pulse ploarity need to be inverted;
	 * bit[8]  : 1, INVVFRAME : VSYNC pulse ploarity need to be inverted;
	 * bit[7]  : 0, INVVD     : VD  pulse ploarity  don't need to be inverted;
	 * bit[6]  : 0, INVVEN    : VDEN pulse ploarity  don't need to be inverted;
	 * bit[5]  : 0, INVPWREN  : PWREN pulse ploarity  don't need to be inverted;
	 * bit[4]  : 0, INVLEND   : 没有用到LEND;
	 * bit[3]  : 0, PWREN     : LCD_PWREN output signal enable/disable;
	 * bit[2]  : 0, ENLEND    : 没有用到LEND;
	 * bit[1]  : 0, BSWP      : 
	 * bit[0]  : 1, HWSWP     :2440手册P413；
	 */
	lcd_regs->lcdcon5 = (1<<11) | (1<<9) | (1<<8) | (1<<0);

	/* 3.3 分配显存framebuffer，并把地址告诉控制器 */
	s3c_lcd->screen_base    = dma_alloc_writecombine (NULL, s3c_lcd->fix.smem_len, &s3c_lcd->fix.smem_start, GFP_KERNEL);
	/* 3.3.1 将地址告诉LCD控制器 */

	lcd_regs->lcdsaddr1 = (s3c_lcd->fix.smem_start>>1) & ~(3<<30);
	lcd_regs->lcdsaddr2 = ((s3c_lcd->fix.smem_start + s3c_lcd->fix.smem_len)>>1) & 0x1fffff;
	lcd_regs->lcdsaddr3 = (480 * 16 / 16);   /* 一行的长度，单位是半字 */

	/* 启动LCD */
	lcd_regs->lcdcon1 |= (1<<0); /* 使能LCD本身 */
	lcd_regs->lcdcon5 |= (1<<3);
	*gpbdat |= 1;                /* 输出高电平,     使能背光灯 */
	/* 4. 注册 */
	register_framebuffer(s3c_lcd);
	
	return 0;
}

static void  lcd_exit(void)
{
	unregister_framebuffer(s3c_lcd);
	lcd_regs->lcdcon1 &= ~(1<<0);    /* 关闭LCD本身 */
	*gpbdat &= ~1;           /* 关闭背光 */
	dma_free_writecombine(NULL, s3c_lcd->fix.smem_len, s3c_lcd->screen_base, s3c_lcd->fix.smem_start);
	iounmap(lcd_regs);
	iounmap (gpbcon);
	iounmap (gpccon);
	iounmap (gpdcon);
	iounmap (gpgcon);
	framebuffer_release(s3c_lcd);
}


module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");

