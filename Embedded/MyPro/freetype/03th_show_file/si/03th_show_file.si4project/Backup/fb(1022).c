// create, set, register a struture

static int FBDeviceInit (void);
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor);
static int FBCleanScreen(unsigned int dwBackColor);

static T_DispOpr g_tFBDispOpr = {
	.name        = "s3c2440_fb",
	.DeviceInit  = FBDeviceInit,
	.ShowPixel   = FBShowPixel,
	.CleanScreen = FBCleanScreen,
};
static int FBDeviceInit (void)
{

}
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{

}
static int FBCleanScreen(unsigned int dwBackColor)
{

}

int FBInit(void)
{
	RegisterDispOpr(&g_tFBDispOpr);
}
