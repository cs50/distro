/****************************************************************************
 * huff.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * David J. Malan
 *
 * Implements Huffman compression.
 ***************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "forest.h"
#include "huffile.h"
#include "tree.h"

// a bit
typedef enum { ZERO, ONE, EOC } bit;

// prototype
void mkcodes(bit codes[][SYMBOLS+1], Tree* tree, bit code[], int ith);

int main(int argc, char * argv[])
{
    // storage for characters
    int c;

    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: %s input output\n", argv[0]);
        return 1;
    }

    // open input
    FILE* input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open %s for reading.\n", argv[1]);
        return 1;
    }

    // initialize header
    Huffeader header;
    header.magic = MAGIC;
    for (int i = 0; i < SYMBOLS; i++)
    {
        header.frequencies[i] = 0;
    }
    header.checksum = 0;

    // count characters in input
    while ((c = fgetc(input)) != EOF)
    {   
        header.frequencies[c]++;
        header.checksum++;
    }

    // ensure file is non-empty
    if (header.checksum == 0)
    {   
        fclose(input);
        printf("Input file is empty.\n");
        return 1;
    }   

    // rewind stream
    rewind(input);

    // create forest
    Forest* forest = mkforest();
    if (forest == NULL)
    {
        fclose(input);
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
            rmforest(forest);
            fclose(input);
            printf("Could not make tree.\n");
            return 1;
        }

        // initialize tree
        tree->symbol = i;
        tree->frequency = header.frequencies[i];

        // plant tree in forest
        if (plant(forest, tree) == false)
        {
            rmforest(forest);
            fclose(input);
            printf("Could not plant tree.\n");
            return 1;
        }
    }

    // build huffman tree
    Tree *tree;
    while (true)
    {
        // pick a tree
        tree = pick(forest);
        if (tree == NULL)
        {
            rmforest(forest);
            fclose(input);
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
            fclose(input);
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
            fclose(input);
            rmforest(forest);
            printf("Could not plant parent.\n");
            return 1;
        }
    }

    // delete forest
    rmforest(forest);

    // prepare for codes
    bit codes[SYMBOLS][SYMBOLS+1];
    for (int i = 0; i < SYMBOLS; i++)
    {
        for (int j = 0; j < SYMBOLS+1; j++)
        {
            codes[i][j] = EOC;
        }
    }

    // generate codes (using 0 for single-node trees)
    if (tree->left == NULL && tree->right == NULL)
    {
        codes[(int) tree->symbol][0] = 0;
    }
    else
    {
        bit code[SYMBOLS+1];
        mkcodes(codes, tree, code, 0);
    }

    // delete tree
    rmtree(tree);

    // open file for output
    Huffile* output = hfopen(argv[2], "w");
    if (output == NULL)
    {
        fclose(input);
        printf("Could not open %s for writing.\n", argv[2]);
        return 1;
    }

    // write header out to file
    if (hwrite(&header, output) == false)
    {
        fclose(input);
        printf("Could not write header.\n");
        return 1;
    }

    // write bits to file
    while ((c = fgetc(input)) != EOF)
    {
        for (int i = 0; codes[c][i] != EOC; i++)
        {
            if (bwrite(codes[c][i], output) == false)
            {
                fclose(input);
                printf("Could not write bit.\n");
                return 1;
            }
        }
    }

    // close files
    hfclose(output);
    fclose(input);

    // that's all folks
    return 0;
}

void mkcodes(bit codes[][SYMBOLS+1], Tree* tree, bit code[], int ith)
{
    // record code if at leaf
    if (tree->left == NULL && tree->right == NULL)
    {
        for (int i = 0; i < ith; i++)
        {
            codes[(unsigned char) tree->symbol][i] = code[i];
        }
        return;
    }

    // encode left subtree
    code[ith] = ZERO;
    mkcodes(codes, tree->left, code, ith+1);

    // encode right subtree
    code[ith] = ONE;
    mkcodes(codes, tree->right, code, ith+1);
}
