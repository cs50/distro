#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("eyale:%s\n", crypt("blue", "50"));
    printf("jharvard:%s\n", crypt("crimson", "50"));
    printf("skroob:%s\n", crypt("12345", "50"));
}
