#include "apue.h"
#include <pwd.h>
#include <stddef.h>
#include <string.h>

struct passwd* getpwnam(const char* name)
{
    struct passwd* ptr;

    setpwent();
    while ((ptr = getpwent()) != NULL)
        if (strcmp(name, ptr->pw_name) == 0)
            break; /* found a match */
    endpwent();
    return (ptr); /* ptr is NULL if no match found */
}
int main(void)
{
    struct passwd *ptr, *ptr1;

    ptr = getpwnam("yjh");
    printf("%s\n", ptr->pw_dir);
    ptr1 = getpwnam("root");
    printf("%s  %#x\n", ptr1->pw_dir, (int)sizeof(struct passwd));
    return 0;
}
