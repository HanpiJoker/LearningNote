#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
/*forthtest*/
int main(int argc, const char *argv[])
{
	int fd;
	unsigned char key_val;
	int ret;

	struct pollfd fds[1];
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
		printf ("can't open!\n");
	fds[0]=fd;
	fds[0].events = POLLIN;
	while (1)
	{
		ret = poll(fds, 1, 5000);
		if (!ret)
		{
			printf("Time OUT!\n");
		}
		else
		{
			read(fd, &key_val, 1);
			printf ("key pressed: 0x%x\n", key_val);
		}
	}
	return 0;
}
