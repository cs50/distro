#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("andi:%s\n", crypt("hi", "50"));
    printf("jason:%s\n", crypt("JH", "50"));
    printf("malan:%s\n", crypt("NOPE", "50"));
    printf("mzlatkova:%s\n", crypt("ha", "50"));
    printf("patrick:%s\n", crypt("Yale", "50"));
    printf("rbowden:%s\n", crypt("rofl", "50"));
    printf("summer:%s\n", crypt("FTW", "50"));
    printf("stelios:%s\n", crypt("ABC", "50"));
    printf("wmartin:%s\n", crypt("haha", "50"));
    printf("zamyla:%s\n", crypt("lol", "50"));
}
