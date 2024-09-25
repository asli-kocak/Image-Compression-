/*
 * CODE_VALUE.H
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 3, 2022
 * 
 * 
 */
#ifndef CODE_VALUE_INCLUDED
#define CODE_VALUE_INCLUDED

#include "video_pixel.h"


/* Stores the calculated codeword values from a 2x2 pixel blocks */
typedef struct Code_values {
    uint16_t a;
    int8_t b, c, d; /* only takes up '4' bits (5 for signed) */
    uint8_t pb, pr; /* only takes up '4' bits */
} Code_values;

Code_values video_to_code_vals(Video_pixel pix1, Video_pixel pix2, 
                               Video_pixel pix3, Video_pixel pix4);

void code_vals_to_video(Code_values block, Video_pixel *pix1, 
                        Video_pixel *pix2, Video_pixel *pix3,
                        Video_pixel *pix4);

#endif 