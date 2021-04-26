#include<stdio.h>

/* i2c_test r addr
 * i2c_test w addr val
 */

void print_usage(char *file)
{
	printf("%s r addr\n", file);
	printf("%s w addr val\n", file);
}

int main(int argc, char **argv)
{
	int fd;

	if ((argc != 3) && (argc != 4)) {
		print_usage (argv[0]);
		return -1;
	}

	if ((fd = open("/dev/at24cxx", O_RDWR)) < 0) {
		printf("can't open /dev/at24cxx\n");
		return -1;
	}

	if (strcmp(argv[1], "r") == 0) {
		buf[0] = strtoul(argv[2], NULL, 0);
		read(fd, buf, 1);
		printf("data: %c, %d, 0x%2x\n", buf[0], buf[0], buf[0]);
	} else if (strcmp(argv[1], "w") == 0) {
		buf[0] = strtoul(argv[2], NULL, 0);
		buf[1] = strtoul(argv[3], NULL, 0);
		write(fd, buf, 2);
	} else {
		print_usage(argv[0]);
		return -1;
	}
	return 0;
}