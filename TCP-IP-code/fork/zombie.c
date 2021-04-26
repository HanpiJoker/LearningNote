#include <stdio.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    pid_t pid = fork();

    if (pid == 0) {
        puts("Hi, I'm a child process");
    } else {
        printf("Child process ID: %d", pid);
        sleep(30);
    }
    if (pid == 0)
        puts("End child process");
    else
        puts("End parent process");
    return 0;
}
