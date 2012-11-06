/****************************************************************************
 * puff.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * David J. Malan
 *
 * Implements Huffman decompression.
 ***************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "forest.h"
#include "huffile.h"
#include "tree.h"

// a bit
typedef enum { ZERO, ONE, EOC } bit;

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: %s input output\n", argv[0]);
        return 1;
    }

    // open input
    Huffile* input = hfopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open %s for reading.\n", argv[1]);
        return 1;
    }

    // read in header
    Huffeader header;
    if (hread(&header, input) == false)
    {
        hfclose(input);
        printf("Could not read header.\n");
        return 1;
    }

    // check for magic number
    if (header.magic != MAGIC)
    {
        hfclose(input);
        printf("File was not huffed.\n");
        return 1;
    }

    // check checksum
    int checksum = header.checksum;
    for (int i = 0; i < SYMBOLS; i++)
    {
        checksum -= header.frequencies[i];
    }
    if (checksum != 0)
    {
        hfclose(input);
        printf("File was not huffed.\n");
        return 1;
    }

    // create forest
    Forest* forest = mkforest();
    if (forest == NULL)
    {
        hfclose(input);
        printf("Could not make forest.\n");
        return 1;
    }

    // plant trees in forest
    for (int i = 0; i < SYMBOLS; i++)
    {
        // don't plant useless trees
        if (header.frequencies[i] == 0)
        {
            continue;
        }

        // make tree
        Tree* tree = mktree();
        if (tree == NULL)
        {
            hfclose(input);
            rmforest(forest);
            printf("Could not make tree.\n");
            return 1;
        }

        // initialize tree
        tree->symbol = i;
        tree->frequency = header.frequencies[i];

        // plant tree in forest
        if (plant(forest, tree) == false)
        {
            hfclose(input);
            rmtree(tree);
            rmforest(forest);
            printf("Could not plant tree.\n");
            return 1;
        }
    }

    // build huffman tree
    Tree* tree;
    while (true)
    {
        // pick a tree
        tree = pick(forest);
        if (tree == NULL)
        {
            hfclose(input);
            rmforest(forest);
            printf("Could not pick any trees.\n");
            return 1;
        }

        // pick another tree
        Tree* sibling = pick(forest);

        // if only one tree in forest, we have our Huffman tree
        if (sibling == NULL)
        {
            break;
        }

        // join trees as siblings
        Tree* parent = mktree();
        if (parent == NULL)
        {
            hfclose(input);
            rmtree(tree);
            rmtree(sibling);
            rmforest(forest);
            printf("Could not make parent.\n");
            return 1;
        }

        // initialize parent
        parent->frequency = tree->frequency + sibling->frequency;
        parent->left = tree;
        parent->right = sibling;

        // add parent to forest
        if (plant(forest, parent) == false)
        {
            hfclose(input);
            rmtree(parent);
            rmforest(forest);
            printf("Could not plant parent.\n");
            return 1;
        }
    }

    // delete forest
    rmforest(forest);

    // open file for output
    FILE* output = fopen(argv[2], "w");
    if (output == NULL)
    {
        hfclose(input);
        rmtree(tree);
        printf("Could not open %s for writing.\n", argv[2]);
        return 1;
    }

    // write out chars to file
    int bit;
    Tree* current = tree;
    while ((bit = bread(input)) != EOF)
    {
        // follow appropriate branch, if any
        if (current->left != NULL && current->right != NULL)
        {
            current = (bit) ? current->right: current->left;
        }

        // print symbol if at leaf
        if (current != NULL)
        {
            if (current->left == NULL && current->right == NULL)
            {
                fwrite(&current->symbol, sizeof(unsigned char), 1, output);
                current = tree;
            }
        }
    }

    // delete tree
    rmtree(tree);

    // close files
    fclose(output);
    hfclose(input);

    // that's all folks
    return 0;
}

