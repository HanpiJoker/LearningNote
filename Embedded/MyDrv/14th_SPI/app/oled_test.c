#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "oledfont.h"

#define OLED_CMD_INIT       0x100001
#define OLED_CMD_CLEAR_ALL  0x100002
#define OLED_CMD_CLEAR_PAGE 0x100003
#define OLED_CMD_SET_POS    0x100004

void OLED_Put_char(int fd, int page, int col, char c)
{
	int i;
	
	// Get Matrix
	const unsigned char *dots = oled_asc2_8x16[c - ' '];
	
	// Send to OLED
	// OLED_Set_Pos(page, col);
	ioctl(fd, OLED_CMD_SET_POS, page | (col << 8));
	write (fd, &dots[0], 8);
	
	// OLED_Set_Pos(page + 1, col);
	ioctl(fd, OLED_CMD_SET_POS, (page + 1) | (col << 8));
	write (fd, &dots[8], 8);
}


void OLED_Print(int fd, int page, int col, char *str)
{
	int i = 0;
	ioctl (fd, OLED_CMD_CLEAR_PAGE, page);	

	while (str[i])
	{
		OLED_Put_char(fd, page, col, str[i]);
		col += 8;
		if (col > 127) {
			col = 0;
			page += 2;
			ioctl (fd, OLED_CMD_CLEAR_PAGE, page);	
		}
		i++;
	}
}



/* oled_test init
 * oled_test cleanall
 * oled_test clear <page>
 * oled_test showin <page> <col> <str>
 */
 

void print_usage(char *str)
{
	printf("Usage: \n");
	printf("%s init \n", str);
	printf("%s cleanall \n", str);
	printf("%s clear <page> \n", str);	
	printf("%s showin <page> <col> <str> \n", str);
	printf("eg:\n");
	printf("%s 2 0 100ask.taobao.com \n", str);
	
}

int main (int argc, char **argv)
{
	int fd;
	int do_what = 0;
	int page = -1, col;
	if ((argc == 2) && !strcmp (argv[1], "init"))
		do_what = OLED_CMD_INIT;
	if ((argc == 2) && !strcmp (argv[1], "cleanall"))
		do_what = OLED_CMD_CLEAR_ALL;	
	if ((argc == 3) && !strcmp (argv[1], "clear")) {
		do_what = OLED_CMD_CLEAR_PAGE;
		page = strtoul(argv[2], NULL, 0);
	}
	if ((argc == 5) && !strcmp (argv[1], "showin")) {
		do_what = OLED_CMD_SET_POS;
		page = strtoul(argv[2], NULL, 0);
		col  = strtoul(argv[3], NULL, 0);
	}

	if (!do_what) {
		print_usage (argv[0]);
		return -1;
	}

	if((fd = open("/dev/spi_oled", O_RDWR)) < 0) {
		printf ("Can't open /dev/spi_oled\n");
		return -1;
	}

	switch (do_what)
	{
		case OLED_CMD_INIT:
		{
			ioctl (fd, OLED_CMD_INIT);
			break;
		}
		
		case OLED_CMD_CLEAR_ALL:
		{
			ioctl (fd, OLED_CMD_CLEAR_ALL);
			break;
		}
		
		case OLED_CMD_CLEAR_PAGE:
		{
			if (page == -1)
				ioctl (fd, OLED_CMD_CLEAR_ALL);
			else if (page < 0 || page > 7) {
				printf ("page only can be 0 ~ 7\n");
				return -1;
			} else 
				ioctl (fd, OLED_CMD_CLEAR_PAGE, page);
			break;
		}
		
		case OLED_CMD_SET_POS:
		{
			if (page < 0 || page > 7) {
				printf ("page only can be 0 ~ 7\n");
				return -1;
			} 		
			if (col < 0 || col > 127) {
            	printf("col only can be 0 ~ 127\n");
            	return -1;
        	}
			OLED_Print(fd, page, col, argv[4]);
			break;
		}
			
	}
	return 0;
}
