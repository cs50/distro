#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("belindazeng:%s\n", crypt("puzzles", "50"));
    printf("caesar:%s\n", crypt("13", "50"));
    printf("jharvard:%s\n", crypt("crimson", "50"));
    printf("malan:%s\n", crypt("password", "50"));
    printf("rob:%s\n", crypt("password", "HA"));
    printf("zamyla:%s\n", crypt("1337", "50"));
    return 0;
}
