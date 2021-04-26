#include<sys/types.h>
#include<sys/stat.h>

#include<fcntl.h>
#include<stdio.h>

int main (int argc, char **argv)
{
    int fd1, fd2;
    unsigned char regval[4];
//    unsigned int regval;
    int i;

    fd1 = open ("/dev/led1", O_RDWR);
    fd2 = open ("/dev/key1", O_RDWR);
    while (1)
    {
        if (fd1 < 0 | fd2 < 0)
        {
            printf ("Can't open!\n");
        }
        read (fd2, &regval, sizeof(regval));
        //printf ("Wow the key_int.ko  work! %d\n", regval);
        write (fd1, &regval, sizeof(regval));
        for (i=0; i<3; i++)
            if (regval[i] == 0)
                printf ("LED %d is on\n", i+4);
        if (regval[3] == 0)
            printf ("All LEDs are on\n");
       
    }
    return 0;
}
