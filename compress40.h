/*
 * COMPRESS40.H
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 1, 2022
 * 
 * 
 * 
 * 
 */ 

#ifndef COMPRESS_INCLUDED
#define COMPRESS_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"

#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"

extern void compress40 (FILE *input);

extern void decompress40(FILE *input);

/*
 * COMPRESSION FUNCTIONS
 */

/* reads in image from file and initializes a Pnm_ppm holding image data */
Pnm_ppm initialize_image(FILE *fp, A2Methods_T methods);

/* Apply func to map over Pnm_ppm; converts each 2x2 pixel block to a
   32-bit word */
void create_block_word(int i, int j, A2Methods_UArray2 array2, void *elem, 
                       void *vcl);


/*
 * DECOMPRESSION FUNCTIONS
 */

/* reads in image header and declares a Pnm_ppm struct to hold data */
struct Pnm_ppm read_compressed_img(FILE *fp);

/* Apply func to map over Pnm_ppm; unpacks and writes each 32-bit word into
   each 2x2 pixel block */
void read_block_word(int i, int j, A2Methods_UArray2 array2, void *elem, 
                       void *vcl);

#endif