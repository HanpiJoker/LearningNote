#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <config.h>
#include <disp_manager.h>


// create, set, register a struture

static int FBDeviceInit (void);
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor);
static int FBCleanScreen(unsigned int dwBackColor);

static int gFB_fd;
static struct fb_var_screeninfo g_tFBVar;	/* Current var */
static struct fb_fix_screeninfo g_tFBFix;	/* Current fix */
static unsigned char *g_pucFBMem;
staic unsigned int g_dwScreenSize;


static unsigned int g_dwLineWidth;
static unsigned int g_dwLinePixelWidth;


static T_DispOpr g_tFBDispOpr = {
	.name        = "s3c2440_fb",
	.DeviceInit  = FBDeviceInit,
	.ShowPixel   = FBShowPixel,
	.CleanScreen = FBCleanScreen,
};
static int FBDeviceInit (void)
{
	int ret;

	
	// refer to 'MyPro\freetype\02th_lcd\show_line.c'
	gFB_fd = open(FB_DEVICE_NAME, O_RDWR);
	if (0 > gFB_fd)
	{
		DBG_PRINT("can't open %s\n", FB_DEVICE_NAME);
		return -1;
	}

	ret = ioctl(gFB_fd, FBIOGET_VSCREENINFO, &g_tFBVar);
	if (ret < 0)
	{
		DBG_PRINT("can't get fb's var\n");
		return -1;
	}

	ret = ioctl(gFB_fd, FBIOGET_FSCREENINFO, &g_tFBFix);
	if (ret < 0)
	{
		DBG_PRINT("can't get fb's fix\n");
		return -1;
	}

	g_dwScreenSize = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;
	g_pucFBMem = (unsigned char *)mmap(NULL, g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if(g_pucFBMem == (unsigned char *)-1) {
		printf("can't mmap\n");
		return -1;
	}
	memset(g_pucFBMem, 0, g_dwScreenSize);

	g_tFBDispOpr.iXres = g_tFBVar.xres;
	g_tFBDispOpr.iYres = g_tFBVar.yres;
	g_tFBDispOpr.iBpp  = g_tFBVar.bits_per_pixel;

	g_dwLineWidth      = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;
	g_dwLinePixelWidth = g_tFBVar.bits_per_pixel / 8;

	
}
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	
	unsigned short wColor16bpp;   // 565

	int iRed;
	int iGreen;
	int iBlue;

	if ((iPenX >= g_tFBVar.xres) || (iPenY >= g_tFBVar.yres))
	{
		DBG_PRINT("can't mmap\n");
		return -1;
	}

	pucFB = g_pucFBMem + y * g_dwLineWidth + x * g_dwLinePixelWidth;
	pwFB16bpp = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;

	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			// 8bits 意味着调色板的索引值
			*pucFB = (unsigned char)dwColor;
			break;
		}
		case 16:
		{
			// 565
			iRed = (dwColor >> (16 + 3)) & 0x1f;
			iGreen = (dwColor >> (8 + 2)) & 0x3f;
			iBlue = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwFB16bpp = wColor16bpp;
			break;
		}
		case 32:
		{
			*pdwFB32bpp = dwColor;
			break;
		}
		default:
		{
			DBG_PRINT("can't support %dbpp\n", g_tFBVar.bits_per_pixel);
			break;
		}
	}

}
static int FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	
	unsigned short wColor16bpp;   // 565

	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;
	
	pucFB = g_pucFBMem + y * g_dwLineWidth + x * g_dwLinePixelWidth;
	pwFB16bpp = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;

	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			// 8bits 意味着调色板的索引值
			memset(g_pucFBMem, dwBackColor, g_dwScreenSize);
			break;
		}
		case 16:
		{
			// 565
			iRed = (dwBackColor >> (16 + 3)) & 0x1f;
			iGreen = (dwBackColor >> (8 + 2)) & 0x3f;
			iBlue = (dwBackColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while(i < g_dwScreenSize)
			{
				*pwFB16bpp = wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while(i < g_dwScreenSize)
			{
				*pdwFB32bpp = dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default:
		{
			DBG_PRINT("can't support %dbpp\n", g_tFBVar.bits_per_pixel);
			break;
		}
	}
}

int FBInit(void)
{
	RegisterDispOpr(&g_tFBDispOpr);
}
