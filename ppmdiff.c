/* 
 * PPMDIFF.C
 *
 * Created by: Skylar Gilfeather and Asli Kocak
 * On: February 25, 2022
 *
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <except.h>
#include <assert.h>
#include <math.h>

#include "pnm.h"

#include "a2plain.h"
#include "a2blocked.h"
#include "a2methods.h"
#include "ppmdiff.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void check_ppm_compressions(Pnm_ppm img_1, Pnm_ppm img_2, A2Methods_T methods);

int main(int argc, char **argv)
{
    
    if (argc != 3) {
        fprintf(stderr, "Usage: ./ppmdiff <image> <image>.\n");
        exit(1);
    }
    /* default to UArray2 methods */
    A2Methods_T methods = uarray2_methods_plain; 
    assert(methods != NULL);

    /* default to best map */
    A2Methods_mapfun *map = methods->map_default; 
    assert(map != NULL);
    
    char *image_1 = argv[1];
    char *image_2 = argv[2];
    FILE *fp1, *fp2;

    if (strcmp(image_1, "-") == 0 || strcmp(image_2, "-") == 0 ) {
        fprintf(stderr, "Cannot read both images in from standard input.\n");
        exit(1);
    }
    
    if (strcmp(image_1, "-") == 0) {
        fp1 = stdin;
    } else {
        fp1 = fopen(image_1, "r");
    }

    if (strcmp(image_2, "-") == 0) {
        fp2 = stdin;
    } else {
        fp2 = fopen(image_2, "r");
    }

    if (fp1 == NULL) {
        fprintf(stderr, "Unable to open file: %s.\n", image_1);
        exit(1);
    }
    if (fp2 == NULL) {
         fprintf(stderr, "Unable to open file: %s.\n", image_2);
    }

    Pnm_ppm image1_ppm = Pnm_ppmread(fp1, methods);
    Pnm_ppm image2_ppm = Pnm_ppmread(fp2, methods);
    
    assert(image1_ppm != NULL);
    assert(image2_ppm != NULL);

    check_ppm_compressions(image1_ppm, image2_ppm, methods);

    return 0;
}

void check_ppm_compressions(Pnm_ppm img_1, Pnm_ppm img_2, A2Methods_T methods)
{
    float width = 0;
    float height = 0;
    float numerator_sum = 0;

    if (abs((int)img_1->width - (int)img_2->width) > 1) {
        fprintf(stderr, "Compressed image widths may differ by at most one. \
                          Difference between %d and %d is too big.\n",
                          img_1->width, img_2->width);
        fprintf(stdout, "1.0");
    }
    if (abs((int)img_1->height - (int)img_2->height) > 1) {
        fprintf(stderr, "Compressed image heights may differ by at most one. \
                          Difference between %d and %d is too big.\n",
                          img_1->height, img_2->height);
        fprintf(stdout, "1.0");
    }
    
    width = (float) MIN(img_1->width, img_2->width);
    height = (float) MIN(img_1->height, img_2->height);
    float denom_1 = img_1->denominator;
    float denom_2 = img_2->denominator;

    assert(width != 0);
    assert(height != 0);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float red, green, blue;
            Pnm_rgb pixel1 = methods->at(img_1->pixels, i, j);
            Pnm_rgb pixel2 = methods->at(img_2->pixels, i, j);

            red = ((float)pixel1->red / denom_1) - 
                  ((float)pixel2->red / denom_2);
            red *= red;
            
            green = ((float)pixel1->green / denom_1) - 
                   ((float)pixel2->green / denom_2);
            green *= green;            
            
            blue = ((float)pixel1->blue / denom_1) - 
                   ((float)pixel2->blue / denom_2);
            blue *= blue;
            
            numerator_sum += red + green + blue;
        }
    }

    float final = sqrt(numerator_sum / (3.0 * width * height));

    printf("E value is: %.4f \n", final);
}

#undef MIN