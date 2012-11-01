#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("caesar:%s\n", crypt("13", "50"));
    printf("cs50:%s\n", crypt("12345678", "50"));
    printf("jharvard:%s\n", crypt("crimson", "50"));
    printf("malan:%s\n", crypt("crimson", "HA"));
    printf("nate:%s\n", crypt("bacon", "50"));
    printf("rbowden:%s\n", crypt("password", "50"));
    printf("skroob:%s\n", crypt("12345", "50"));
    printf("tmacwilliam:%s\n", crypt("n00b", "50"));
    printf("zamyla:%s\n", crypt("l33t", "50"));
    return 0;
}
