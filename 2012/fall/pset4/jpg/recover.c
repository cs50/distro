/*************************************************************************
 * recover.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Recovers JPEG files from a RAW file.
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// length of the output filenames (e.g. "001.jpg")
#define FILENAME_LENGTH 8

// block size of 
#define BLOCK_SIZE 512

// JPEG header format
#define SOI_0  0xff
#define SOI_1  0xd8
#define APPN_0 0xff
#define APP0_1 0xe0
#define APP1_1 0xe1

/**
 * Returns true iff a JPEG header.
 */
bool is_jpeg_header(unsigned char header[])
{
    return (header[0] == SOI_0 && header[1] == SOI_1 &&
            header[2] == APPN_0 && (header[3] == APP0_1 ||
                                    header[3] == APP1_1));
}

int main(int argc, char* argv[])
{
    if (argc != 1)
    {
        printf("Usage: recover\n");
        return 1;
    }
    
    FILE* rawfile = fopen("card.raw", "r");
    if (rawfile == NULL)
    {
		printf("recover: %s: No such file\n", argv[1]);
		return 1;
    }

    // these will track and store each individual JPEG as we find it
    char jpeg_filename[FILENAME_LENGTH];
    int num_jpegs_recovered = 0;
    FILE* jpeg_file = NULL;

    // make *sure* to use an *unsigned* char buffer, otherwise the hex
    // comparisons with 0xff and the like won't work!
    unsigned char buffer[BLOCK_SIZE];

    // we want to read BLOCK_SIZE bytes into our buffer, no more and no
    // less, since the RAW file is guaranteed to be a multiple of
    // BLOCK_SIZE bytes in length
    while (fread(buffer, 1, BLOCK_SIZE, rawfile) == BLOCK_SIZE)
    {
        // check the first 4 bytes of the buffer for a JPEG header
        if (is_jpeg_header(buffer))
        {
            // if we were writing a JPEG, time to close it
            if (jpeg_file != NULL)
            {
                fclose(jpeg_file);
            }

            // use sprintf to generate a custom filename
            sprintf(jpeg_filename, "%03d.jpg", num_jpegs_recovered++);

            // open up a file for the JPEG we just found using + to create
            jpeg_file = fopen(jpeg_filename, "w+");
            if (jpeg_file == NULL)
            {
                fclose(rawfile);
                printf("recover: %s: Error creating file\n", jpeg_filename);
                return 1;
            }
        }
        
        // if we've got a JPEG open, then write data to it
        if (jpeg_file != NULL)
        {
            // make sure that the write succeeds
            if (fwrite(buffer, 1, BLOCK_SIZE, jpeg_file) != BLOCK_SIZE)
            {
                fclose(jpeg_file);
                fclose(rawfile);
                printf("recover: %s: Error writing file\n", jpeg_filename);
                return 1;
            }
        }
    }

    // close the last JPEG file we were writing
    if (jpeg_file != NULL)
    {
        fclose(jpeg_file);
    }

    // check to see if we stopped reading because of an error
    if (ferror(rawfile))
    {
        fclose(rawfile);
        printf("recover: %s: Error reading file\n", argv[1]);
        return 1;
    }

    // otherwise, all good!
    fclose(rawfile);

    return 0;
}
