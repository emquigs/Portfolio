/*
 *
 * ppmdiff reads in two ppm images and compares them
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"

static A2Methods_T methods;
static A2Methods_mapfun *map;
typedef A2Methods_UArray2 A2;

double calculate_diff (Pnm_ppm i1, Pnm_ppm i2);
    
double sumPixels(Pnm_rgb p1, Pnm_rgb p2, int d1, int d2);

int main(int argc, char *argv[])
{
    Pnm_ppm img_1, img_2;
    float diff;
    FILE *fp1;
    FILE *fp2;

    methods = uarray2_methods_plain;

    map = methods->map_default;

    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        exit(1);
    } else if (argc == 3) {
        fp1 = fopen(argv[1], "rb");
        fp2 = fopen(argv[2], "rb");
        img_1 = Pnm_ppmread(fp1, methods);
        img_2 = Pnm_ppmread(fp2, methods);
    } else if (argc == 2) {
        fp1 = fopen(argv[1], "rb");
        img_1 = Pnm_ppmread(fp1, methods);
        img_2 = Pnm_ppmread(stdin, methods);
    }

    if ((methods->width(img_1) - methods->width(img_2)) > 1 || (methods->width(img_1) - methods->width(img_2)) < -1) {
        diff = 1;
        fprintf(stderr, "widths have difference bigger than 1\n");
        printf("%5f\n", diff);
        exit(1);
    } else if ((methods->width(img_1) - methods->width(img_2)) > 1 || (methods->width(img_1) - methods->width(img_2)) < -1) {
        diff = 1;
        fprintf(stderr, "heights have difference bigger than 1\n");
        printf("%5f\n", diff);
        exit(1);
    } 

    diff = calculate_diff(img_1, img_2);

    printf("%4f\n", diff);
    fclose(fp1);
    fclose(fp2);

    exit(0);
}

double calculate_diff (Pnm_ppm i1, Pnm_ppm i2)
{
    unsigned w, h;

    if(i1->width < i2->width) {
           w = i1->width;
    } else {
           w = i2->width;
    }

    if(i1->height < i2->height) {
           h = i1->height;
    } else {
           h = i2->height;
    }
    int sum = 0;
        for (unsigned rows = 0; rows < h; rows++) {
            for (unsigned cols = 0; cols < w; cols++) {
                sum += sumPixels(methods->at(i1->pixels, cols, rows), methods->at(i2->pixels, cols, rows), i1->denominator, i2->denominator);
            }
        }
        printf("d1: %d, d2: %d \n", i1->denominator, i2->denominator);
    return sqrt(sum / (3 * w * h));
}
    
double sumPixels(Pnm_rgb p1, Pnm_rgb p2, int d1, int d2) {
    double dee1 = (double)d1;
    double dee2 = (double)d2;

    double red = ( ((double) p1->red)/dee1 - ((double)p2->red)/dee2) * (((double)p1->red)/dee1 - ((double)p2->red)/dee2);
    double green = (p1->green/dee1 - p2->green/dee2) * (p1->green/dee1 - p2->green/dee2);
    double blue = (p1->blue/dee1 - p2->blue/dee2) * (p1->blue/dee1 - p2->blue/dee2);

    return red + green + blue;
}

