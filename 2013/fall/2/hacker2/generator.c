#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("caesar:%s\n", crypt("13", "50"));
    printf("hirschhorn:%s\n", crypt("password", "50"));
    printf("jharvard:%s\n", crypt("crimson", "50"));
    printf("malan:%s\n", crypt("crimson", "HA"));
    printf("milo:%s\n", crypt("1337", "HA"));
    printf("zamyla:%s\n", crypt("1337", "50"));
    return 0;
}
