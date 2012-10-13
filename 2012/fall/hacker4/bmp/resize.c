/****************************************************************************
 * resize.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Resizes a BMP by a factor of n on (0.0, 100.0).
 ***************************************************************************/
       
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
	 printf("Usage: resize f infile outfile\n");
	 return 1;
    }

    // ensure resize factor is a float between 0.0 and 100.0
    for (int i = 0, len = strlen(argv[1]); i < len; i++)
    {
        if (!isdigit(argv[1][i]) && argv[1][i] != '.')
        {
            printf("the second argument must be an integer.\n");
            return 5;
        }
    }

    // initialize resize factor.
    float factor = atof(argv[1]);

    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // range check: n must be on (0.0, 100.0)
    if (factor <= 0 || factor >= 100)
    {
        printf("The resize factor, must satisfy 0 < n <= 100.\n");
        return 6;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
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
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // dimensions of original for future reference
    int oldWidth  = bi.biWidth;
    int oldHeight = abs(bi.biHeight);

    // determine padding for scanlines of original
    int oldPadding =  (4 - (oldWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update BITMAPINFOHEADER with dimensions after resize
    bi.biWidth  *= factor;
    bi.biHeight *= factor;

    // determine padding for scanlines of resized image
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update BITMAPINFOHEADER with new image size
    bi.biSizeImage = ((3 * bi.biWidth) + padding) * abs(bi.biHeight);

    // update BITMAPFILEHEADER with new bitmap file size
    bf.bfSize = bi.biSizeImage + 54;

    // write outfile's BITMAPFILEHEADER and BITMAPINFOHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // currRow stores one of the rows in the input picture.
    // rowNum states which row is being stored.
    RGBTRIPLE* currRow = malloc(oldWidth * sizeof(RGBTRIPLE));
    if (currRow == NULL)
        return 7;
    int rowNum = -1;

    // iterate over every pixel in outfile, and find and write
    // the new color.
    for (int y = 0; y < abs(bi.biHeight); y++)
    {
        // Nearest neighbors.  All of the pixels on one
        // outfile row will draw from the pixels of one row
        // in the infile.  Find out what row that should be and
        // load it into currRow, if we haven't already.
        int oldY = (int)(y / factor);

        // load rows from the input picture one by one until we
        // get the correct one.
        while (rowNum < oldY)
        {
            rowNum++;
            for (int j = 0; j < oldWidth; j++)
            {
                RGBTRIPLE triple;
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                currRow[j] = triple;
            }

            // skip over padding, if any
            fseek(inptr, oldPadding, SEEK_CUR);
        }

        // Draw the pixels in that row into the output.
        for (int x = 0; x < bi.biWidth; x++)
        {
            RGBTRIPLE triple = currRow[(int)(x / factor)];
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // write padding for scanline to outfile
        for (int l = 0; l < padding; l++)
            fputc(0x00, outptr);
    }

    free( currRow );
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // kthxbai
    return 0;
}
