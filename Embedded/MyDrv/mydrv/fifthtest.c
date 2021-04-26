#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/*fifthtest*/
int fd;

void my_signal_fun (int signum)
{
	unsigned char key_val;
	read (fd, &key_val, 1);
	printf ("key_val = 0x%x\n", key_val);
}

int main(int argc, const char *argv[])
{
	
	unsigned char key_val;
	int ret, oflags;

	signal (SIGIO, my_signal_fun);
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf ("can't open!\n");
	}

	fcntl (fd, F_SETOWN, getpid());
	oflags = fcntl (fd, F_GETFL);
	fcntl (fd, F_SETFL, oflags | FASYNC);

	while (1)
	{
		sleep (10000);
	}
	return 0;
}
