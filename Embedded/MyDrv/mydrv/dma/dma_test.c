#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>

/* ./dma_test nodma
 * ./dma_test dma
 */
#define MEM_COPY_NO_DMA   0
#define MEM_COPY_DMA      1

void print_usage (char *name)
{
	printf ("Usage:\n");
	printf ("%s <nodma | dma>\n", name);
}

int main (int argc, char ** argv)
{
	int fd;
	
	if (argc != 2)
	{
		print_usage(argv[0]);
		return -1;
	}

	fd = open ("/dev/s3c_dma0", O_RDWR);
	if (fd < 0)
	{
		printf ("Can't open /dev/dma\n");
		return -1;
	}
	if (!strcmp (argv[1], "nodma") )
	{
		while (1)
		{
			ioctl (fd, MEM_COPY_NO_DMA);
		}
	}
	else if (!strcmp (argv[1], "dma") )
	{
		while (1)
		{
			ioctl (fd, MEM_COPY_DMA);
		}
	}
	else
	{
		print_usage(argv[0]);
		return -1;
	}
}
