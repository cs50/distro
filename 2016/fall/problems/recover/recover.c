/**
 * recover.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Recovers JPEG files from a RAW file.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// length of the output filenames (e.g. "001.jpg")
#define JPEG_FILENAME_LENGTH 8

// format of the output JPEG filenames
#define JPEG_FILENAME_FORMAT "%03i.jpg"

// block size of a CF card, in bytes
#define BLOCK_SIZE 512

/**
 * Returns true iff we've got a JPEG header as defined in the pset
 * specification.
 */
bool is_jpeg_header(unsigned char header[])
{
    return (header[0] == 0xff && 
            header[1] == 0xd8 && 
            header[2] == 0xff &&
            (header[3] & 0xf0) == 0xe0);
}

int main(int argc, char* argv[])
{
    // usage
    if (argc != 2)
    {
        printf("Usage: recover image\n");
        return 1;
    }

    FILE *raw_file = fopen(argv[1], "r");
    if (raw_file == NULL)
    {
        printf("recover: %s: Error opening file\n", argv[1]);
        return 1;
    }

    // these will track and store each individual JPEG as we find it
    char jpeg_filename[JPEG_FILENAME_LENGTH];
    int num_jpegs_recovered = 0;
    FILE *jpeg_file = NULL;

    // make *sure* to use an *unsigned* char buffer, otherwise the hex
    // comparisons with 0xff and the like won't work!
    unsigned char buffer[BLOCK_SIZE];

    // we want to read BLOCK_SIZE bytes into our buffer, no more and no
    // less, since the RAW file is guaranteed to be a multiple of
    // BLOCK_SIZE bytes in length
    while (fread(buffer, 1, BLOCK_SIZE, raw_file) == BLOCK_SIZE)
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
            sprintf(jpeg_filename, JPEG_FILENAME_FORMAT, num_jpegs_recovered++);

            // open up a file for the JPEG we just found using + to create
            jpeg_file = fopen(jpeg_filename, "w+");
            if (jpeg_file == NULL)
            {
                fclose(raw_file);
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
                fclose(raw_file);
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
    if (ferror(raw_file))
    {
        fclose(raw_file);
        printf("recover: %s: Error reading file\n", argv[1]);
        return 1;
    }

    // otherwise, all good!
    fclose(raw_file);
    return 0;
}
