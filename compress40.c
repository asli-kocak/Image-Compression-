/*
 * COMPRESS40.C
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 1, 2022
 * 
 * HEADER
 * 
 *  What is the problem rn: it reads all things until the block row one from 
 *  the last but it makes no sense

 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "assert.h"
#include "uarray.h"

#include "a2methods.h"
#include "a2blocked.h"
#include "pnm.h"
#include "compress40.h"
#include "seq.h"

#include "video_pixel.h"
#include "code_value.h"

/* GLOBAL VALUES */
A2Methods_T methods;
Seq_T global_sequence;
int denom;
int global_i = 0;

static void apply_reset_elements(int i, int j, A2Methods_UArray2 array2, 
                                 A2Methods_Object *ptr, void *cl);


/* not using right now */
// static uint32_t create_block_word_helper(Seq_T pix_seq);

static void fill_block_word_test(int i, int j, A2Methods_UArray2 array2, 
                          void *elem, void *cl);

static void test_block_word_codevals(Seq_T pixels);

/*
 * The two functions below are functions you should implement.
 * They should take their input from the parameter and should
 *  write their output to stdout
 */

extern void compress40 (FILE *input)  /* read PPM, write compressed image */
{
    assert(input != NULL);
    /* The file is opened outside */
    
    A2Methods_T methods = uarray2_methods_blocked;
    Pnm_ppm ppm = initialize_image(input, methods);

    struct Pnm_ppm new_image = {.width = ppm->width, .height = ppm->height, 
                                .denominator = ppm->denominator,
                                .pixels = ppm->pixels, 
                                .methods = methods
    };

    global_sequence = Seq_new(ppm->width * ppm->height);
    denom = ppm->denominator;

    Seq_T pnm_pixels = Seq_new(4);
    

    methods->map_block_major(ppm->pixels, create_block_word,
                             &pnm_pixels);

    methods->map_block_major(new_image.pixels, fill_block_word_test, NULL);

    Pnm_ppmwrite(stdout, &new_image);
}

extern void decompress40(FILE *input)  /* read compressed image, write PPM */
{
    assert(input != NULL);
    /* read in compressed image header */
    struct Pnm_ppm decompress_img = read_compressed_img(input);

    (void) decompress_img;

    /* map over UArray2b pixels with each consecutive word read in */
}

/* 
 * COMPRESSION AND DECOMPRESSION FUNCTIONS 
 */

/* initialize_image()
 * 
 * Validates formatting, and creates a ppm image from the given
 * file input. Trims width and height to be even, and returns 
 * the Ppm_pnm instance it’s read into.
 *
 * @param	FILE *fp	        File pointer to image data
 * @param   A2Methods methods   Method suite for UArray2b methods
 * @returns instance of Ppm_pnm with given data. If width and/or
 * height is odd, trims last row and/or column to make them even.
 * If Pnm_ppmread fails to initialize the Pnm_ppm, returns NULL.
 * 
 * @expects		FILE *fp passed in is non-null and opened
 * 				successfully. 
 */
Pnm_ppm	initialize_image(FILE *fp, A2Methods_T methods)
{
    Pnm_ppm image = Pnm_ppmread(fp, methods);
    A2Methods_UArray2 pixels = image->pixels;

    if (image == NULL) {
        return NULL;
    }

    int width = image->width;
    int height = image->height;

    /* */
    width = (image->width % 2 != 0) ? width - 1 : width;
    height = (image->height % 2 != 0) ? height - 1 : height;

    A2Methods_UArray2 small_pixels = methods->new_with_blocksize(width, 
                                        height, sizeof(struct Pnm_rgb), 2);
    
    /* overwrite new smaller A2UArray2b with old values */
    methods->map_block_major(small_pixels, apply_reset_elements, image);

    methods->free(&pixels);
    image->pixels = small_pixels;

    image->width = width;
    image->height = height;

    return image;
}

/* apply_reset_elements() 
 *
 * A helper function passed to the A2Methods' map_block_major() for UArray2b.
 * Given a populated A2Methods_UArray2, passed into void *cl, writes the value
 * at (i, j) in cl into the element *ptr at coordinates (i, j) in the 
 * A2Methods_UArray2 array2.
 * 
 * @param   int i                       Int column coordinate to write ot
 * @param   int j                       Int row coordinate to write to
 * @param   A2Methods_UArray2 array2    Uninialized A2Methods_UArray2 to fill
 * @param   A2Methods_Object *ptr       Reference to element at (i, j)
 * @param   void *cl                    Populated A2Methods_UArray2
 * 
 * @returns none, but populates the A2Methods_UArray2 array2 with cl's data
 * 
 * @expects
 */ 
static void apply_reset_elements(int i, int j, A2Methods_UArray2 array2, 
                                 A2Methods_Object *ptr, void *cl)
{
    (void) array2;

    Pnm_ppm our_ppm = (Pnm_ppm) cl;
    struct Pnm_rgb *small_curr_pixel = (Pnm_rgb)ptr;
    
    void *old_curr_pixel = our_ppm->methods->at(our_ppm->pixels, i, j);
    *small_curr_pixel = (*(Pnm_rgb)old_curr_pixel);

}

/* read_compressed_img()
 * 
 * Given a file pointer to the compressed image data, read the 
 * file in 32-bit blocks 
 *
 * @param		File pointer to compressed image
 *
 * @returns	a ppm_pnm instance with width, height, and denominator set		
 * 
 * @expects	If the supplied file is too short (i.e., the 
 * 			number of codewords is too low for the stated 
 * 			width and height, or the last one is incomplete), 
 * 			throws Checked Runtime Error. 
 *
 */
struct Pnm_ppm read_compressed_img(FILE *fp)
{
    unsigned height, width;
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                      &width, &height);
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');

    A2Methods_T methods = uarray2_methods_blocked;
    A2Methods_UArray2 pixels = methods->new_with_blocksize(width, 
                                        height, sizeof(struct Pnm_rgb), 2);

    struct Pnm_ppm pixmap = { .width = width, .height = height, 
                              .denominator = 255, .pixels = pixels, 
                              .methods = methods
    };
      
    return pixmap;
}

 
/* create_block_word()
 * 
 * A mapping function, to be called on the PPM image’s A2UArray,a 
 * UArray2b separated into blocks of 2 Pnm_rgb by 2 Pnm_rgb. 
 *
 * @param	int i			  Column of current pixel in A2Uarray 
 * @param	int j			  Row of current pixel in A2Uarray 
 * @param	A2UArray array2	  A2Uarray containing PPM image data
 * @param	void elem		  Void pointer with address of element
 * @param   void *vcl		  Contains a UArray_T of Pnm_rgb pixels with
 *                            length 4; will contain previous pixels within
 *                            the current block. 
 *
 * @returns 			      void, but prints each 32-bit word calculated
 *                            to stdout through calls to ------()
 * 
 * @expects		  Bounds are handled by map_block_major implementation for
 *                UArray2b.
 *                *cl is non-null, containing an initialized UArray_T of
 *                length 4. 
 */
void create_block_word(int i, int j, A2Methods_UArray2 array2, 
                       void *elem, void *cl)
{
    //fprintf(stderr, "Creating block word \n");

    uint32_t word;

    Seq_T pix_seq = *( (Seq_T *)cl );
    assert(pix_seq != NULL);

    struct Pnm_rgb *curr_pix = (struct Pnm_rgb *)elem;
    assert(curr_pix != NULL);

    Seq_addhi(pix_seq, curr_pix);
    
    if (Seq_length(pix_seq) == 4) {
        test_block_word_codevals(pix_seq);
        assert(Seq_length(pix_seq) == 0);
    }

    (void) i;
    (void) j;
    (void) array2;
    (void) word;
}


static void test_block_word_codevals(Seq_T pix_seq) 
{
    assert(pix_seq != NULL);
    Video_pixel vid_array[4];
    Video_pixel new_vid_array[4];
    Video_pixel null_pix = { 0.0, 0.0, 0.0 };
    

    for (int i = 0; i < 4; i++) {
        struct Pnm_rgb *curr_pixel = Seq_get(pix_seq, 0);
        Seq_remlo(pix_seq);
        vid_array[i] = convert_rgb_to_video(*curr_pixel, denom);
        new_vid_array[i] = null_pix;
    }

    Code_values code_vals = video_to_code_vals(vid_array[0], vid_array[1],
                                               vid_array[2], vid_array[3]);

    code_vals_to_video(code_vals, &new_vid_array[0], &new_vid_array[1],
                        &new_vid_array[2], &new_vid_array[3]);

    //fprintf(stderr, "In function test_block_codevals %d \n", global_i);
    global_i++;

    for (int i = 0; i < 4; i++) {
        struct Pnm_rgb pixel = convert_video_to_rgb(new_vid_array[i], denom);   
        Seq_addlo(global_sequence, &pixel);
    }

}

/*
static uint32_t create_block_word_helper(Seq_T pix_seq)
{
    (void) pix_seq;
    assert(pixels != NULL);
    Video_pixel vid_array[4];

    for(int i = 0; i < 4; i++) {
        struct Pnm_rgb curr_pixel = *( (Pnm_rgb)UArray_at(pixels, i) );
        vid_array[i] = convert_rgb_to_video(curr_pixel, denom);
    }
    Code_values code_vals = video_to_code_vals(vid_array[1], vid_array[2],
                                               vid_array[3], vid_array[4]);

    (void) code_vals;
    return 0;
} */

/* read_block_word()
 * 
 * A mapping function, to be called on the PPM image’s A2UArray,a 
 * UArray2b separated into blocks of 2 Pnm_rgb by 2 Pnm_rgb.
 * Read a Pnm_rgb from the front of a Seq of Pnm_rgbs into the
 * given index of the A2Uarray.
 *
 * @param	int i			  Column of current pixel in A2Uarray 
 * @param	int j			  Row of current pixel in A2Uarray 
 * @param	A2UArray array2	  A2Uarray containing PPM image data
 * @param	void elem		  Void pointer with address of element
 * @param  void *vcl		  Void pointer to a Seq of Pnm_rgb
 * 					  pixels, organized from front to back
 * 					  in block-major order as read from 
 * 					  file. 
 *
 * @returns 			  void, but writes Pnm_rb struct from
 * 					      *vcl into the given indices.  
 * 
 * @expects		  Bounds are handled by map_block_major 
 * 				  implementation for UArray2b. Expects *vcl 
 * 				  to be non-null. 
 */
void read_block_word(int i, int j, A2Methods_UArray2 array2, void *elem, 
                       void *cl)
{
    (void) i;
    (void) j;
    (void) array2;
    (void) elem;
    (void) cl;
    /* read */
}

void fill_block_word_test(int i, int j, A2Methods_UArray2 array2, 
                          void *elem, void *cl)
{
    struct Pnm_rgb pixel_got;
    //fprintf(stderr, "i: %u, j: %u\n", i, j);
    struct Pnm_rgb *pixel_spot = (struct Pnm_rgb *)elem;

    // if (Seq_length(global_sequence) == 0){
    //    // fprintf(stderr, "length 0, i = %d and j = %d \n", i,j);
    // }

    // }  else {
    //         pixel_got = (struct Pnm_rgb *)Seq_remhi(global_sequence);
    // }
    pixel_got = *((Pnm_rgb)Seq_remhi(global_sequence));
    
    pixel_spot->red = pixel_got.red;
    pixel_spot->green = pixel_got.green; 
    pixel_spot->blue = pixel_got.blue;
    fprintf(stderr, "red: %u, green: %u, blue: %u\n\n", pixel_got.red,
                                                        pixel_got.green,
                                                        pixel_got.blue);
    
    //free(pixel_got);
    (void) i;
    (void) j;
    (void) array2;
    (void) cl;
}