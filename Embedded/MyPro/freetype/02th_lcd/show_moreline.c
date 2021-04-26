#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

int fb_fd;
struct fb_var_screeninfo var;	/* Current var */
struct fb_fix_screeninfo fix;	/* Current fix */

int screen_size;
unsigned char *fbmem;
unsigned int line_width;
unsigned int pixel_width;
 
void lcd_put_pixel(int x, int y, unsigned int color)
{
	unsigned char *pen_8 = fbmem + y * line_width + x * pixel_width;
	unsigned short *pen_16;
	unsigned int *pen_32;

	unsigned int red, green, blue;

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (var.bits_per_pixel)
	{
		case 8:
		{
			// 8bits 意味着调色板的索引值
			*pen_8 = color;
			break;
		}
		case 16:
		{
			// 565
			red = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue = color & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't support %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
}

void draw_bitmap( FT_Bitmap*  bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;

  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= var.xres || j >= var.yres )
        continue;

      // image[j][i] |= bitmap->buffer[q * bitmap->width + p];
      lcd_put_pixel(i, j, bitmap->buffer[q * bitmap->width + p]);
    }
  }
}



int main (int argc, char **argv)
{
	wchar_t *wstr1 = L"龍的传人gif";
	wchar_t *wstr2 = L"MyLoveOfYou";
	FT_Library	  library;
	FT_Face 	  face;
	FT_GlyphSlot  slot;
	FT_Vector	  pen;
	FT_Matrix	  matrix;				  /* transformation matrix */

	FT_BBox bbox;
	FT_Glyph glyph;

	double		  angle;
	FT_Error	  error;
	int           i;

	int line_ybox_min = 10000;
	int line_ybox_max = 0;
	
	if ( argc != 3 )
	{
	  fprintf ( stderr, "usage: %s <font> <angle>\n", argv[0] );
	  exit( 1 );
	}

	fb_fd = open("/dev/fb0", O_RDWR);
	if(fb_fd < 0) {
		printf("can't open /dev/fb0\n");
		return -1;
	}

	if(ioctl(fb_fd, FBIOGET_VSCREENINFO, &var)) {
		printf("can't get var\n");
		return -1;
	}
	
	if(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix)) {
		printf("can't get fix\n");
		return -1;
	}

	line_width = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;

	fbmem = (unsigned char *)mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
	if(fbmem == (unsigned char *)-1) {
		printf("can't mmap\n");
		return -1;
	}
 
	memset(fbmem, 0, screen_size);

	// Show the vector font
	
	error = FT_Init_FreeType( &library );			   /* initialize library */
	/* error handling omitted */
	
	error = FT_New_Face( library, argv[1], 0, &face ); /* create face object */
	/* error handling omitted */

	FT_Set_Pixel_Sizes(face, 24, 0);
	slot = face->glyph;

	/* x = 0,  
	 * y = 24,
	 * turn to cartesian coordinate system
	 * x = lcd_x = 0;
	 * y = target_height - lcd_y = var.yres - 24;
	 */
	pen.x = (0) * 64;
	pen.y = ( var.yres- 24) * 64;

	angle = ( 1.0 * strtoul(argv[2], NULL, 0) / 360 ) * 3.14159 * 2;	  /* use 25 degrees 	*/

	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	for(i = 0; i < wcslen(wstr1); i++)
	{
		/* set transformation */
	    FT_Set_Transform( face, &matrix, &pen );

	    /* load glyph image into the slot (erase previous one) */
	    error = FT_Load_Char( face, wstr1[i], FT_LOAD_RENDER );
	    if ( error ) {
			printf("FT_Load_Char error!\n");
			return -1;
		}
		
		error = FT_Get_Glyph(face->glyph, &glyph);
		if (error) {
			printf("FT_Get_Glyph error!\n");
			return -1;
		}
		
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox);
		if (line_ybox_min > bbox.yMin)
			line_ybox_min = bbox.yMin;
		if (line_ybox_max < bbox.yMax)
			line_ybox_max = bbox.yMax; 
		draw_bitmap( &slot->bitmap,
		     		  slot->bitmap_left,
				      var.yres - slot->bitmap_top );
		    /* increment pen position */
	    pen.x += slot->advance.x;
	    pen.y += slot->advance.y;
	}

	
	/* x = 0,  
	 * y = line_ybox_max - line_ybox_min + 24,
	 * turn to cartesian coordinate system
	 * x = lcd_x = 0;
	 * y = target_height - lcd_y = var.yres - (line_ybox_max - line_ybox_min + 24);
	 */
	pen.x = (0) * 64;
	pen.y = (var.yres - (line_ybox_max - line_ybox_min + 24)) * 64;

	angle = ( 1.0 * strtoul(argv[2], NULL, 0) / 360 ) * 3.14159 * 2;	  /* use 25 degrees 	*/

	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	for(i = 0; i < wcslen(wstr2); i++)
	{
		/* set transformation */
	    FT_Set_Transform( face, &matrix, &pen );

	    /* load glyph image into the slot (erase previous one) */
	    error = FT_Load_Char( face, wstr2[i], FT_LOAD_RENDER );
	    if ( error ) {
			printf("FT_Load_Char error!\n");
			return -1;
		}
		
		error = FT_Get_Glyph(face->glyph, &glyph);
		if (error) {
			printf("FT_Get_Glyph error!\n");
			return -1;
		}
		
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_TRUNCATE, &bbox);
		if (line_ybox_min > bbox.yMin)
			line_ybox_min = bbox.yMin;
		if (line_ybox_max < bbox.yMax)
			line_ybox_max = bbox.yMax; 
		draw_bitmap( &slot->bitmap,
		     		  slot->bitmap_left,
				      var.yres - slot->bitmap_top );
		    /* increment pen position */
	    pen.x += slot->advance.x;
	    pen.y += slot->advance.y;
	}
	return 0;	
}
