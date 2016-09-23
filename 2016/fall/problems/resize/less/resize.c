/**
 * resize.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Resizes a BMP by a factor of n on (0, 100].
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: resize n infile outfile\n");
        return 1;
    }

    // ensure resize factor is an int
    for (int i = 0, len = strlen(argv[1]); i < len; i++)
    {
        if (!isdigit(argv[1][i]))
        {
            printf("n, the resize factor, must be an integer.\n");
            return 1;
        }
    }

    // initialize resize factor
    int n = atoi(argv[1]);

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // range check: n must be on (0, 100]
    if (n <= 0 || n > 100)
    {
        printf("n, the resize factor, must satisfy 0 < n <= 100.\n");
        return 1;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 1;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 1;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        printf("Unsupported file format.\n");
        return 1;
    }

    // dimensions of original for future reference
    int oldWidth  = bi.biWidth;
    int oldHeight = abs(bi.biHeight);

    // determine padding for scanlines of original
    int oldPadding =  (4 - (oldWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update BITMAPINFOHEADER with dimensions after resize
    bi.biWidth *= n;
    bi.biHeight *= n;

    // determine padding for scanlines of resized image
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update BITMAPINFOHEADER with new image size
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding) * abs(bi.biHeight);

    // update BITMAPFILEHEADER with new bitmap file size
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER)
                + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < oldHeight; i++)
    {
        // array stores in memory all pixels in scanline
        RGBTRIPLE currRow[oldWidth];

        // read a scanline from the infile
        fread(&currRow, sizeof(RGBTRIPLE), oldWidth, inptr);

        // write new RGB triples to outfile for next n scanlines
        for (int k = 0; k < n; k++)
        {
            for (int l = 0; l < oldWidth; l++)
            {
                // apply old triple to next n pixels on scanline
                for (int m = 0; m < n; m++)
                {
                    fwrite(&currRow[l], sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // write padding for scanline to outfile
            for (int l = 0; l < padding; l++)
            {
                fputc(0x00, outptr);
            }
        }

        // skip over padding, if any
        fseek(inptr, oldPadding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
