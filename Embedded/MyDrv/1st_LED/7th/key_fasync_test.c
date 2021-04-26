#include<sys/types.h>
#include<sys/stat.h>
#include<poll.h>
#include<fcntl.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>

int fd1, fd2;

static void signalio_handler (int signum)
{
    unsigned char regval[4];
    int i;

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

int main (int argc, char **argv)
{
	int oflags;

    fd1 = open ("/dev/led1", O_RDWR);
    fd2 = open ("/dev/key1", O_RDWR);

	signal (SIGIO, signalio_handler);

	fcntl(fd2, F_SETOWN, getpid());
	oflags = fcntl (fd2, F_GETFL);
	fcntl (fd2, F_SETFL, oflags | FASYNC);
	
    while (1)
    {
		sleep(100);
	}       
    return 0;
}

