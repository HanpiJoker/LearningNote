#include <stdio.h>
int main(int argc, const char *argv[])
{
    if ( -1 < (unsigned char)1 ) {
        printf("-1 is less than (unsigned char)1 : ANSI semantics");
    } else {
        printf("-1 isn't less than (unsigned char)1 : K&R semantics");
    }        
    return 0;
}
