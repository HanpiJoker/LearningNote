#include <stdio.h>
#include <time.h>

int main(int argc, const char *argv[])
{
    int bits_time = sizeof(time_t) * 8;
    time_t wrap_time = (0x1 << (bits_time - 1)) - 1;
    printf("bits of time: %d\n", bits_time);
    printf("ctime output: %s\n", ctime(&wrap_time));
    printf("gmtime output:%s\n", asctime(gmtime(&wrap_time)));
    printf("BeiJing Is Faster 8 Hours Than UTC\n");
    return 0;
}
