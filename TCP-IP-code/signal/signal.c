#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if(sig == SIGALRM)
        puts("Time out");
    alarm(2);
}

void keycontrol(int sig)
{
    if(sig == SIGINT)
        puts("Ctrl+C pressed");
}

int main(int argc, const char *argv[])
{
    int i;
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(2);

    for(i =0; i<3;i++)
    {
        printf("wait...\n");
        sleep(100);  
    }
    return 0;
}
