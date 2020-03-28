/* 
Copyright (C) 2016-2020 Biswajit Roy

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define DEBUG 0
#define MAX_INTENSITY 256

/*Structure for BMP Header*/
struct bmpheader {
    unsigned char id1;                 // first 2-bytes for BM ID field [1-byte]
    unsigned char id2;                 //                  ;;           [1-byte]
    unsigned int size;                 // Size of the BMP file
    unsigned short int app_spec_1;     // Application specific
    unsigned short int app_spec_2;     // Application specific
    unsigned int offset;               // Offset where the pixel array (bitmap data) can be found
};


/*Structure for DIB [Device Independent Bitmap] Header*/
struct dibheader {
    unsigned int size;                  // Number of bytes in the DIB header (from this point)
    int width;                          // Width of the bitmap in pixels
    int height;                         // Height of the bitmap in pixels. Positive for bottom to top pixel order
    unsigned short int color_planes;    // Number of color planes being used
    unsigned short int bits_per_pixel;  // Number of bits per pixel
    unsigned int compression;           // BI_RGB, pixel array compression used
    unsigned int size_with_padding;     // Size of the raw bitmap data (including padding)
    unsigned int resolution_horizontal; // resolution of the image (horizontal)
    unsigned int resolution_vertical;   // resolution of the image (vertical)
    unsigned int color_palette;         // Number of colors in the palette
    unsigned int important_colors;      // Number of important colors; 0 means all colors are important
};


struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


int BMPHistEq(struct color *image, int width, int height) {
    int *pixel_arr = (int *) malloc(width*height*sizeof(int));
    int i, max_pixel_value=0;
    double temp=0.0; 
    int color_map[MAX_INTENSITY] = {0};   // map of intensity values from 0-255
    double PMF[MAX_INTENSITY] = {0.0};
    double CDF[MAX_INTENSITY] = {0.0};
    int DF[MAX_INTENSITY] = {0};
    int total_pixels = width*height;

    #ifdef DEBUG
        // printf("Size of pixel array: %d\n\n", sizeof(*pixel_arr));
    #endif
    
    for(i=0;i<height*width; i++) {
        *(pixel_arr+i) = image[i].r;    // red pixel only
    }

    #ifdef DEBUG
        // printf("\nPrinting pixel array:\n");
        // for(i=0;i<height*width; i++) {
        //     printf("%d ", *pixel_arr);
        //     pixel_arr++;
        // }
    #endif

    // Count frequencies of pixel values in 0-255
    // and finding max pixel
    for(i=0; i<height*width; i++) {
        if(max_pixel_value < *(pixel_arr+i)) 
            max_pixel_value = *(pixel_arr+i);
        color_map[*(pixel_arr+i)]++;
    }

    printf("\nMax pixel value is %d", max_pixel_value);
    
    for(i=0; i<MAX_INTENSITY; i++) {
        PMF[i] = (double) color_map[i]/max_pixel_value;
    }

    // Calculate CDF (Cumulative Distributive Function)
    for(i=0; i<MAX_INTENSITY; i++) {
        temp = temp + PMF[i];
        CDF[i] = temp;
    }


    // CDF value with (Gray levels (minus) 1) 
    for(i=0; i<MAX_INTENSITY; i++) {
        temp = temp + PMF[*(pixel_arr+i)];
        DF[i] = (int) round(max_pixel_value * CDF[i]);
    }

    // print density function array...
    // for(i=0; i<width*height; i++) {
    //     printf("\n%d : %d", *(pixel_arr+i), DF[*(pixel_arr+i)]);
    // }

    // print CDF array...
    printf("\n\n");
    for(i=0; i<MAX_INTENSITY; i++) {
        printf("%d ", DF[i]);
    }

    free(pixel_arr);
    return 0;
}


void printColor(struct color *image, int width, int height) {
    long long int image_size = sizeof(*image)*width*height;
    int i=0;
    while(i<image_size) {
        printf("%d ", (image+i)->r);
        i++;
    }
    printf("\n\nTotal count of pixels: %d\n", i);
}



int main() {
    char filename[100];                 // Holds input BMP Image filename
    FILE *fp;
    int i,j;                            // Loop variables
    int status;
    printf("*****************************************************************\n");
    printf("\tHistogram Equalization on BMP Image (24-Bit non alpha)\n");
    printf("*****************************************************************\n\n");
    printf("\nEnter a BMP image filename: ");
    scanf("%s", filename);

    if((fp = fopen(filename, "rb"))==NULL) {    // Open file in read as byte mode
        fclose(fp);
        printf("\nError, cannot open file.\nTerminating...\n");
        return 0;
    }


    struct bmpheader header0;
    struct dibheader header1;
    struct color *image;

    fread(&header0.id1, sizeof(header0.id1), 1, fp);
    fread(&header0.id2, sizeof(header0.id2), 1, fp);
    fread(&header0.size, sizeof(header0.size), 1, fp);
    fread(&header0.app_spec_1, sizeof(header0.app_spec_1), 1, fp);
    fread(&header0.app_spec_2, sizeof(header0.app_spec_2), 1, fp);
    fread(&header0.offset, sizeof(header0.offset), 1, fp);
    
    fread(&header1.size, sizeof(header1.size), 1, fp);
    fread(&header1.width, sizeof(header1.width), 1, fp);
    fread(&header1.height, sizeof(header1.height), 1, fp);
    fread(&header1.color_planes, sizeof(header1.color_planes), 1, fp);
    fread(&header1.bits_per_pixel, sizeof(header1.bits_per_pixel), 1, fp);
    fread(&header1.compression, sizeof(header1.compression), 1, fp);
    fread(&header1.size_with_padding, sizeof(header1.size_with_padding), 1, fp);
    fread(&header1.resolution_horizontal, sizeof(header1.resolution_horizontal), 1, fp);
    fread(&header1.resolution_vertical, sizeof(header1.resolution_vertical), 1, fp);
    fread(&header1.color_palette, sizeof(header1.color_palette), 1, fp);
    fread(&header1.important_colors, sizeof(header1.important_colors), 1, fp);

    

    printf("\n\nBMP Header Statistics:");
    printf("\nID feild = %c%c\nSize of BMP file = %lu\nApplication specific = %d\nApplication specific = %d\nOffset where the pixel array (bitmap data) can be found = %lu\n", header0.id1, header0.id2, header0.size, header0.app_spec_1, header0.app_spec_2, header0.offset);

    printf("\n\nDIB Header Statistics:");
    printf("\nNumber of bytes in the DIB header = %lu\nWidth of the bitmap in pixels = %d\nHeight of the bitmap in pixels = %d\nNumber of color planes being used = %d\nNumber of bits per pixel = %d\nNumber of color planes being used = %d\nNumber of bits per pixel = %d\nBI_RGB, pixel array compression used = %lu\nSize of the raw bitmap data (including padding) = %lu\nResolution of the image (horizontal) = %lu\nResolution of the image (vertical) = %lu\nNumber of colors in the palette = %lu\nNumber of important colors; 0 means all colors are important = %lu\n", header1.size, header1.width, header1.height, header1.color_planes, header1.bits_per_pixel, header1.size_with_padding, header1.resolution_horizontal, header1.resolution_vertical, header1.color_palette, header1.important_colors);

    printf("Debug: File pointer is at %d bytes\n", ftell(fp));


    image=(struct color *)malloc(header1.width*header1.height*sizeof(struct color));    // Allocate memory for pixel array
    fseek(fp,54,SEEK_SET);      // Start of pixel array (bitmap data) -- Optional statement; already at 54th bytes

    // Populating image data...
    for(i=0;i<header1.height;i++)
        for(j=0;j<header1.width;j++)
            fread((image+i*header1.width+j),sizeof(struct color),1,fp);
    
    fclose(fp);                  // Close file pointer
    
    #ifdef DEBUG
        // printColor(image, header1.width, header1.height);
    #endif
    status = BMPHistEq(image, header1.width, header1.height);
    if(status == -1) printf("\nFailed to successfully convert image\n");
    else printf("\nSuccessfully equalized image\n");

    return 0;
}