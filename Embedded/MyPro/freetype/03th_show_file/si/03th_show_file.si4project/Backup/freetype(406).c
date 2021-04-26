#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


static int FreeTypeFontInit(char *pcFontFile, unsigned int dwFontSize);
static int FreeTypeGetFontBitmap(unsigned int dwCode, int iPenX, int iPeny, PT_FontBitMap ptFontBitMap);


static FT_Library	  g_tLibrary;
static FT_Face 	      g_tFace;
static FT_GlyphSlot   g_tSlot;


// create, set, register a struture T_FontOpr
static T_FontOpr g_tFreeTypeFontOpr = {
	.name          = "freetype",
	.FontInit      = FreeTypeFontInit,
	.GetFontBitmap = FreeTypeGetFontBitmap,
};

static int FreeTypeFontInit(char *pcFontFile, unsigned int dwFontSize)
{
	FT_Error iError;

	// Show the vector font
	
	iError = FT_Init_FreeType( &g_tLibrary );			   /* initialize library */
	if (iError)
	{
		DBG_PRINT("FT_Init_FreeType error!\n");
		return -1;
	}
	iError = FT_New_Face( g_tLibrary, argv[1], 0, &g_tFace ); /* create face object */
	if (iError)
	{
		DBG_PRINT("FT_New_Face error!\n");
		return -1;
	}

	g_tSlot = g_tFace->glyph;
	
	return 0;
}

static int FreeTypeGetFontBitmap(unsigned int dwCode, int iPenX, int iPeny, PT_FontBitMap ptFontBitMap)
{

}

int FreeTypeInit(void)
{
	return RegisterFontOpr (&g_tFreeTypeFontOpr);
}
