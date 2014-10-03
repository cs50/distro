/**
 * peek.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Peeks at a BMP's headers.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // -v
    if (argc == 2 && strcmp(argv[1], "-v") == 0)
    {
        printf("1.0\n");
        return 0;
    }

    // ensure proper usage
    if (argc != 2 && argc != 3)
    {
        printf("Usage: peek file [file]\n");
        return 1;
    }

    // allocate 2 of everything
    FILE* files[2];
    BITMAPFILEHEADER fileheaders[2];
    BITMAPINFOHEADER infoheaders[2];

    // open files
    for (int i = 0; i < argc - 1; i++)
    {
        // open file
        files[i] = fopen(argv[i + 1], "r");
        if (files[i] == NULL)
        {
            printf("Could not open %s.\n", argv[i + 1]);
            return 2;
        }

        // read file's BITMAPFILEHEADER
        fread(&fileheaders[i], sizeof(BITMAPFILEHEADER), 1, files[i]);

        // read file's BITMAPINFOHEADER
        fread(&infoheaders[i], sizeof(BITMAPINFOHEADER), 1, files[i]);

        // close file 
        fclose(files[i]);

        // ensure file is (likely) a 24-bit uncompressed BMP 4.0
        if (fileheaders[i].bfType != 0x4d42 || 
            fileheaders[i].bfOffBits != 54 ||
            infoheaders[i].biSize != 40 || 
            infoheaders[i].biBitCount != 24 ||
            infoheaders[i].biCompression != 0)
        {
            fprintf(stderr, "Could not recognize %s.\n", argv[i + 1]);
            return 3;
        }
    }

    if (argc == 2)
    {
        printf("offset  type   name              argv[1]\n");
        printf("0       WORD   bfType           %8.4x\n", fileheaders[0].bfType);
        printf("2       DWORD  bfSize           %8.8x\n", fileheaders[0].bfSize);
        printf("6       WORD   bfReserved1      %8.4x\n", fileheaders[0].bfReserved1);
        printf("8       WORD   bfReserved2      %8.4x\n", fileheaders[0].bfReserved2);
        printf("10      DWORD  bfOffBits        %8.8x\n", fileheaders[0].bfOffBits);
        printf("14      DWORD  biSize           %8.8x\n", infoheaders[0].biSize);
        printf("18      LONG   biWidth          %8.8x\n", infoheaders[0].biWidth);
        printf("22      LONG   biHeight         %8.8x\n", infoheaders[0].biHeight);
        printf("26      WORD   biPlanes         %8.4x\n", infoheaders[0].biPlanes);
        printf("28      WORD   biBitCount       %8.4x\n", infoheaders[0].biBitCount);
        printf("30      DWORD  biCompression    %8.8x\n", infoheaders[0].biCompression);
        printf("34      DWORD  biSizeImage      %8.8x\n", infoheaders[0].biSizeImage);
        printf("38      LONG   biXPelsPerMeter  %8.8x\n", infoheaders[0].biXPelsPerMeter);
        printf("42      LONG   biYPelsPerMeter  %8.8x\n", infoheaders[0].biYPelsPerMeter);
        printf("46      DWORD  biClrUsed        %8.8x\n", infoheaders[0].biClrUsed);
        printf("50      DWORD  biClrImportant   %8.8x\n", infoheaders[0].biClrImportant);
    }
    else
    {
        printf("offset  type   name              argv[1]   argv[2]\n");
        printf("%s%-8i%-7s%-15s%10.4x%10.4x%s\n", (fileheaders[0].bfType == fileheaders[1].bfType) ? "\033[32m" : "\033[31m", 0, "WORD", "bfType", fileheaders[0].bfType, fileheaders[1].bfType, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (fileheaders[0].bfSize == fileheaders[1].bfSize) ? "\033[32m" : "\033[31m", 2, "DWORD", "bfSize", fileheaders[0].bfSize, fileheaders[1].bfSize, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.4x%10.4x%s\n", (fileheaders[0].bfReserved1 == fileheaders[1].bfReserved1) ? "\033[32m" : "\033[31m", 6, "WORD", "bfReserved1", fileheaders[0].bfReserved1, fileheaders[1].bfReserved1, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.4x%10.4x%s\n", (fileheaders[0].bfReserved2 == fileheaders[1].bfReserved2) ? "\033[32m" : "\033[31m", 8, "WORD", "bfReserved2", fileheaders[0].bfReserved2, fileheaders[1].bfReserved2, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (fileheaders[0].bfOffBits == fileheaders[1].bfOffBits) ? "\033[32m" : "\033[31m", 10, "DWORD", "bfOffBits", fileheaders[0].bfOffBits, fileheaders[1].bfOffBits, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biSize == infoheaders[1].biSize) ? "\033[32m" : "\033[31m", 14, "DWORD", "biSize", infoheaders[0].biSize, infoheaders[1].biSize, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biWidth == infoheaders[1].biWidth) ? "\033[32m" : "\033[31m", 18, "LONG", "biWidth", infoheaders[0].biWidth, infoheaders[1].biWidth, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biHeight == infoheaders[1].biHeight) ? "\033[32m" : "\033[31m", 22, "LONG", "biHeight", infoheaders[0].biHeight, infoheaders[1].biHeight, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.4x%10.4x%s\n", (infoheaders[0].biPlanes == infoheaders[1].biPlanes) ? "\033[32m" : "\033[31m", 26, "WORD", "biPlanes", infoheaders[0].biPlanes, infoheaders[1].biPlanes, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.4x%10.4x%s\n", (infoheaders[0].biBitCount == infoheaders[1].biBitCount) ? "\033[32m" : "\033[31m", 28, "WORD", "biBitCount", infoheaders[0].biBitCount, infoheaders[1].biBitCount, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biCompression == infoheaders[1].biCompression) ? "\033[32m" : "\033[31m", 30, "DWORD", "biCompression", infoheaders[0].biCompression, infoheaders[1].biCompression, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biSizeImage == infoheaders[1].biSizeImage) ? "\033[32m" : "\033[31m", 34, "DWORD", "biSizeImage", infoheaders[0].biSizeImage, infoheaders[1].biSizeImage, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biXPelsPerMeter == infoheaders[1].biXPelsPerMeter) ? "\033[32m" : "\033[31m", 38, "LONG", "biXPelsPerMeter", infoheaders[0].biXPelsPerMeter, infoheaders[1].biXPelsPerMeter, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biYPelsPerMeter == infoheaders[1].biYPelsPerMeter) ? "\033[32m" : "\033[31m", 42, "LONG", "biYPelsPerMeter", infoheaders[0].biYPelsPerMeter, infoheaders[1].biYPelsPerMeter, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biClrUsed == infoheaders[1].biClrUsed) ? "\033[32m" : "\033[31m", 46, "DWORD", "biClrUsed", infoheaders[0].biClrUsed, infoheaders[1].biClrUsed, "\033[39m");
        printf("%s%-8i%-7s%-15s%10.8x%10.8x%s\n", (infoheaders[0].biClrImportant == infoheaders[1].biClrImportant) ? "\033[32m" : "\033[31m", 50, "DWORD", "biClrImportant", infoheaders[0].biClrImportant, infoheaders[1].biClrImportant, "\033[39m");
    }

    // that's all folks
    return 0;
}
