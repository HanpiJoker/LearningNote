#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

FILE *popen2(const char *command, const char *type, pid_t *pid)
{
#define READ 0
#define WRITE 1
	pid_t child_pid;	
	int fd[2];
	pipe(fd);
	if ((child_pid = vfork()) == -1) {
		perror("fork");
		exit(1);
	}

	if (child_pid == 0) {
		if (!strncmp(type, "r", 1)) {
			printf("%d\n", __LINE__);
			close(fd[READ]);
			dup2(fd[WRITE], STDOUT_FILENO);
		} else {
			close(fd[WRITE]);
			dup2(fd[READ], STDIN_FILENO);
		}

		printf("%d\n", __LINE__);
		setpgid(child_pid, child_pid);
		printf("%d\n", __LINE__);
		execl("/bin/sh", "/bin/sh", "-c", command, NULL);
		printf("%d\n", __LINE__);
		_exit(0);
	} else {
		if (!strncmp(type, "r", 1)) {
			close(fd[WRITE]);
		} else {
			close(fd[READ]);
		}
	}
	*pid = child_pid;
	printf("%d\n", __LINE__);

	if (!strncmp(type, "r", 1)) {
		return fdopen(fd[READ], "r");
	}
	
	return fdopen(fd[WRITE], "w");
}

int main(int argc, char *argv[])
{
	pid_t pid;
	FILE *fp;
	char test[100];
	fp = popen2("sudo pacman -Syu", "r", &pid);
	while(fscanf(fp, "%s", test) == 1) {
		printf("%s\n", test);
	}
	return 0;
}
