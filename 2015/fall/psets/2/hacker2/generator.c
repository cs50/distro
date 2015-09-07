#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("andi:%s\n", crypt("Berkeley", "HA"));
    printf("caesar:%s\n", crypt("13", "50"));
    printf("eli:%s\n", crypt("yale", "50"));
    printf("hdan:%s\n", crypt("boola", "50"));
    printf("jason:%s\n", crypt("Silliman", "50"));
    printf("john:%s\n", crypt("harvard", "50"));
    printf("levatich:%s\n", crypt("PBJ", "50"));
    printf("rob:%s\n", crypt("password", "50"));
    printf("skroob:%s\n", crypt("12345", "50"));
    printf("zamyla:%s\n", crypt("password", "HA"));
}
