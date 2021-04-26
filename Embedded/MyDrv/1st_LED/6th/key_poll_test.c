#include<sys/types.h>
#include<sys/stat.h>
#include<poll.h>
#include<fcntl.h>
#include<stdio.h>

int main (int argc, char **argv)
{
    int fd1, fd2;
    unsigned char regval[4];
    int i;
	int ret;
	struct pollfd fds[1];

    fd1 = open ("/dev/led1", O_RDWR);
    fd2 = open ("/dev/key1", O_RDWR);

	fds[0].fd = fd2;
	fds[0].events = POLLIN;
	
    while (1)
    {
		ret = poll(fds, 1, 5000);
		if (!ret)
			printf ("time out\n");
		else {
			if (fd1 < 0 | fd2 < 0)
				printf ("Can't open!\n");
			read (fd2, regval, sizeof(regval));
			write (fd1, regval, sizeof(regval));
			for (i=0; i<3; i++)
				if (regval[i] == 0)
					printf ("LED %d is on\n", i+4);
			if (regval[3] == 0)
				printf ("All   LEDs are on\n");
		}       
    }
    return 0;
}

