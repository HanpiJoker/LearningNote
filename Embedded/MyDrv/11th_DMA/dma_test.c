#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<string.h>

#define MEM_COPY_NO_DMA 0
#define MEM_COPY_DMA 1

void printf_usage (char *name)
{
	printf("Usage:\n");
	printf("%s <nodma | dma>\n", name);
}

int main (int argc, char **argv)
{
	int fd;
	
	if (argc != 2) {
		printf_usage (argv[0]);
		return -1;
	}
	fd = open("/dev/dma", O_RDWR);
	if (fd < 0) {
		printf ("can't open /dev/dma\n");
		return -1;
	}
	
	if (strcmp(argv[1], "nodma") == 0) {
		while (1)
		{
			ioctl(fd,MEM_COPY_NO_DMA);
		}
	} else if (strcmp(argv[1], "dma") == 0) {
		while (1)
		{
			ioctl(fd, MEM_COPY_DMA);
		}
	} else {
		printf_usage(argv[0]);
		return -1;
	}
	
	return 0;
}