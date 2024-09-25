/*
 * CODE_VALUE.C
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 2, 2022
 * 
 * 
 */
#include <stdlib.h>
#include "code_value.h"
#include "arith40.h"
#include "math.h"

static int8_t quantize_bcd(float coeff);

/* Compression
 * video_to_code_vals()
 * 
 * Converts a 2*2 group of 4 PPM image pixels (converted to
 * component video float values) into aggregated averaged data. 
 *
 * @param	Video_pixel pix1	4 Video_pixel struct instances
 * @param	Video_pixel pix2	containing every 4 adjacent pixel
 * @param	Video_pixel pix3	values in one 2*2 block.
 * @param	Video_pixel pix4
 *
 * @returns  			  a Code_values struct, containing Pb  
 * 					  average, Pr average, and scaled 
 * 					  cosine coefficients a, b, c, and d.
 * 
 * @expects			  4 Video_pixels set to be packed
 * 					  together. 
 */
Code_values video_to_code_vals(Video_pixel pix1, Video_pixel pix2, 
                               Video_pixel pix3, Video_pixel pix4)
{
    float a, b, c, d;
    float pb_av, pr_av;
    
    /* video pixel values are calculated properly */
    /* Transforming from pixel space to DCT space: */
    a = (pix4.y + pix3.y + pix2.y + pix1.y) / 4.0;
    b = (pix4.y + pix3.y - pix2.y - pix1.y) / 4.0;
    c = (pix4.y - pix3.y + pix2.y - pix1.y) / 4.0;
    d = (pix4.y - pix3.y - pix2.y + pix1.y) / 4.0;

    pb_av = (pix1.pb + pix2.pb + pix3.pb + pix4.pb) / 4.0; 
    pr_av = (pix1.pr + pix2.pr + pix3.pr + pix4.pr) / 4.0;  

    Code_values vals = { .a  = (uint16_t)round((double)(511 * a)), 
                                .b  = quantize_bcd(b),
                                .c  = quantize_bcd(c),
                                .d  = quantize_bcd(d),
                                .pb = Arith40_index_of_chroma(pb_av),
                                .pr = Arith40_index_of_chroma(pr_av)
    };
    return vals;
}


/* Helper function, contract goes here (TODO) :) */
static int8_t quantize_bcd(float coeff)
{   
    coeff = (float)round((double)coeff * 50);
    if (coeff < -15) {
        coeff = 15;
    }
    if (coeff > 15) {
        coeff = 15;
    }

    return coeff;
}


/* Decompression
 * code_vals_to_video()
 *
 * Given a Code_values struct instance, which contains data for 
 * 4 pixels, returns the pixel values in a Video_pixel struct 
 * instance by using the conversion math provided. Since all the
 * values of the 2*2 block will be the same, they will be written 
 * 4 times in future steps.
 *
 * @param Code_values		Contains info for 2*2 block
 *
 * @returns	Video_pixel struct with pixel values	
 * 
 * @expects	The Code_values block is non-null
 *
 */
void code_vals_to_video(Code_values block, Video_pixel *pix1, 
                               Video_pixel *pix2, Video_pixel *pix3,
                               Video_pixel *pix4)
{    
    float f_a = (float) block.a;
    float f_b = (float) block.b;
    float f_c = (float) block.c;
    float f_d = (float) block.d;

    pix1->y = (f_a / 511.0) - ((f_b - f_c + f_d) / 50.0);
    pix2->y = (f_a / 511.0) - ((f_b + f_c - f_d) / 50.0);
    pix3->y = (f_a / 511.0) + ((f_b - f_c - f_d) / 50.0);
    pix4->y = (f_a / 511.0) + ((f_b + f_c + f_d) / 50.0);

    pix1->pb = Arith40_chroma_of_index(block.pb);
    pix2->pb = Arith40_chroma_of_index(block.pb);
    pix3->pb = Arith40_chroma_of_index(block.pb);
    pix4->pb = Arith40_chroma_of_index(block.pb);
    
    pix1->pr = Arith40_chroma_of_index(block.pr);
    pix2->pr = Arith40_chroma_of_index(block.pr);
    pix3->pr = Arith40_chroma_of_index(block.pr);
    pix4->pr = Arith40_chroma_of_index(block.pr);
}