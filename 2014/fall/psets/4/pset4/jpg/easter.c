#include <stdio.h>
#include <string.h>

int main(void)
{
    FILE* file = fopen("out", "wb");
    char* url = "bit.ly/8v94Kw ;)";
    fprintf(file, "%s", url);
    for (int i = strlen(url); i < 512; i++)
        fprintf(file, "%c", '\0');
    fclose(file);
}
