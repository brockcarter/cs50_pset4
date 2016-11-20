/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }
   
    // remember filenames
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // checks n for proper int
    if(n < 0 || n > 100)
    {
        fprintf(stderr, "Unsupported n for resize. Must be an int between 1 and 100.\n");
        return 5;
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
        fclose(inptr);
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
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // create bitmap headers for outptr
    int origHeight = abs(bi.biHeight);
    int origWidth = bi.biWidth;
    
    //inject new data with regard to n
    bi.biWidth = (bi.biWidth)*n;
    bi.biHeight = (bi.biHeight)*n;
    
    // determine padding for scanlines
    int in_padding =  (4 - (origWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int out_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determine file and image size
    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + out_padding) * abs(bi.biHeight);
    bf.bfSize = 54 + bi.biSizeImage;

    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < origHeight; i++)
        {
            // write each scanline n times
            for (int i = 0; i < n; i++)
                {
                    // iterate over pixels in scanline
                    for (int j = 0; j < origWidth; j++)
                        {
                        // temporary storage
                        RGBTRIPLE triple;

                        // read RGB triple from infile
                        fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                        // write RGB triple to outfile, n times
                        for(int m = 0; m < n; m++)
                            {
                                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);  
                            }
                        }
                
                    // skip over padding in infile, if any
                    fseek(inptr, in_padding, SEEK_CUR); 
            
                    // add correct padding to outfile
                    for (int k = 0; k < out_padding; k++)
                        {
                            fputc(0x00, outptr);
                        }
                    
                    // brings the cursor back to beginning of scanline in infile 
                    fseek(inptr, -(origWidth * 3 + in_padding), SEEK_CUR);    
                }
            fseek(inptr, origWidth * 3 + in_padding, SEEK_CUR);   
        }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
