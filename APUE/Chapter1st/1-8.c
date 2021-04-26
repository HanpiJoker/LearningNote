#include "apue.h"
#include <errno.h>

int main(int argc, const char *argv[])
{
    fprintf (stderr, "EACCES: %s\n", strerror(EACCES));
    errno = ENOENT;
    perror (argv[0]);
    exit (0);
}
