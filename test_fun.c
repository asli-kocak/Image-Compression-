#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <bool.h>
#include <time.h>
#include "assert.h"

#include "pnm.h"
#include "compress40.h"

#include "video_pixel.h"
#include "code_value.h"

/* VIDEO_PIXEL.H TESTS*/
void big_video_tests();

void test_video_rgb(float y, float pb, float pr);

void test_rgb_vid(unsigned r, unsigned g, unsigned b);

void test_pixs();
int generate_num(int bound1, int bound2);
void test_bitpack ();


/* CODE_VALUE.H TESTS */
void big_codeval_tests();

float get_pixel_diff(struct Pnm_rgb pix1, struct Pnm_rgb pix2, 
                     struct Pnm_rgb pix3, struct Pnm_rgb pix4);

static struct Pnm_rgb pix_gen();

int main(int argc, char **argv) {
    test_pixs();
    test_bitpack();
    (void) argc;
    (void) argv;
    return 0;
}

/*
 * TESTING FUNCTIONS
 */

/*
 * CONTRACT HERE XOXO
 */
void big_video_tests()
{
    for (int i = 0; i < 10; i++){
        unsigned x, y, z;
        x = rand() % 256;
        y = rand() % 65535;
        z = rand() % 65535;
        fprintf(stderr, "Test %u: red: %u, green: %u, blue: %u\n", i, x, y, z);
        test_rgb_vid(x, y, z);
    }

    for (int i = 0; i < 10; i++){
        unsigned x, y, z;
        x = rand() % 255;
        y = rand() % 255;
        z = rand() % 255;

        struct Pnm_rgb newpix = { .red = x, .green = y, .blue = z };

        struct Video_pixel pix = convert_rgb_to_video(newpix, 255);
        fprintf(stderr, "Test %u: y: %f, pb: %f, pr: %f\n", i,
                        pix.y, pix.pb, pix.pr);
        test_video_rgb(pix.y, pix.pb, pix.pr);

    }
}


/*
 * CONTRACT HERE XOXO
 */
void test_video_rgb(float x, float y, float z)
{

    Video_pixel vpix1;
    Video_pixel vpix2;
    vpix1.y = x;
    vpix1.pb = y;
    vpix1.pr = z;
    
    struct Pnm_rgb new_r_pix = convert_video_to_rgb(vpix1, 255);
    vpix2 = convert_rgb_to_video(new_r_pix, 255);
    fprintf(stderr, "        y: %f , pb: %f, pr: %f\n", 
                             vpix2.y, vpix2.pb, vpix2.pr);
    //assert(fabs(vpix2.y - vpix1.y) < .01);
}

/*
 * CONTRACT HERE XOXO
 */
void test_rgb_vid(unsigned x, unsigned y, unsigned z)
{
    Video_pixel vid1;
    
    struct Pnm_rgb rgb1 = { .red  = x, .green = y, .blue = z };

    vid1 =  convert_rgb_to_video(rgb1, 255);
    struct Pnm_rgb rgb2 = convert_video_to_rgb(vid1, 255);

    assert(rgb2.red == rgb1.red);
    assert(rgb2.blue == rgb1.blue);
    assert(rgb2.green == rgb1.green);
}
 
static void print_code_values(Code_values codes) {
    printf("a: %d, b: %d, c: %d, d: %d, pb: %u, pr: %u\n\n",
            codes.a, codes.b, codes.c, codes.d, codes.pb, codes.pr);
}

/*
 * compute_pixel_diff()
 *
 * Similar to check_ppm_compressions() in ppmdiff.c, this calculates the
 * difference between a group of four Pnm_rgb pixels before and after
 * converting them to Code_values structs and back again. 
 */
float get_pixel_diff(struct Pnm_rgb pix1, struct Pnm_rgb pix2, 
                         struct Pnm_rgb pix3, struct Pnm_rgb pix4)
{
    float numerator_sum = 0.0;
    float red_diff = 0.0, green_diff = 0.0, blue_diff = 0.0;

    struct Pnm_rgb old_pix[4] = { pix1, pix2, pix3, pix4 };
    struct Pnm_rgb new_pix[4];

    /* RGB to VIDEO to CODE VAL and BACK */

    Video_pixel video_pixels[4];
    Video_pixel new_vid_pix[4];
    /* convert these pixels to Codevals and back again */
    for (int i = 0; i < 4; i++) {
        Video_pixel vid_pix = convert_rgb_to_video(old_pix[i], 256);
        video_pixels[i] = vid_pix; /*
        Video_pixel placeholder = { .y = 0.0, .pb = 0.0, .pr = 0.0 };
        new_vid_pix[i] = placeholder; */
    }
    
    Code_values code = video_to_code_vals(video_pixels[0],  video_pixels[1], 
                                          video_pixels[2], video_pixels[3]);
    
    print_code_values(code);

    /* ISSUE IS HERE XOXO */
    code_vals_to_video(code, &new_vid_pix[0],  &new_vid_pix[1], 
                             &new_vid_pix[2], &new_vid_pix[3]);
    
    for (int i = 0; i < 3; i++) {
        new_pix[i] = convert_video_to_rgb(new_vid_pix[i], 256);
    }

    /* CALCULATE DIFFERENCE */

    for (int i = 0; i < 10; i++) {
        red_diff = ((float)old_pix[i].red / 256.0) - 
                    ((float)new_pix[i].red / 256.0);
        red_diff *= red_diff;

        green_diff = ((float)old_pix[i].green / 256.0) - 
                    ((float)new_pix[i].green / 256.0);
        green_diff *= green_diff;    
        
        blue_diff = ((float)old_pix[i].blue / 256.0) - 
                    ((float)new_pix[i].blue / 256.0);
        blue_diff *= blue_diff;

        numerator_sum = numerator_sum + red_diff + green_diff + blue_diff;
    }

    float sqrd = sqrtf( numerator_sum / 12.0 );
    return sqrd;
}

/* Generates a random Pnm_rgb instance */
static struct Pnm_rgb pix_gen() {
    struct Pnm_rgb new_pixel;
    new_pixel.red   = (unsigned)(rand() % 256);
    new_pixel.green = (unsigned)(rand() % 256);
    new_pixel.blue  = (unsigned)(rand() % 256);
    
    return new_pixel;
}

void test_pixs()
{
    float diff = 0.0;

    for (int i = 0; i < 5; i++) {
        struct Pnm_rgb pix1 = pix_gen();    
        struct Pnm_rgb pix2 = pix_gen();
        struct Pnm_rgb pix3 = pix_gen();    
        struct Pnm_rgb pix4 = pix_gen();
        
        diff = get_pixel_diff(pix1, pix2, pix3, pix4); 
        if (diff > 1 || i == 1) {
            printf("red: %u, green: %u, blue: %u\n", pix1.red, pix1.green, 
                                                     pix1.blue);
            printf("red: %u, green: %u, blue: %u\n", pix2.red, pix2.green, 
                                                     pix2.blue);  
            printf("red: %u, green: %u, blue: %u\n", pix3.red, pix3.green, 
                                                     pix3.blue);
            printf("red: %u, green: %u, blue: %u\n", pix4.red, pix4.green, 
                                                    pix4.blue);                                                                                                         
        }
        fprintf(stderr, "\nPixel block diff: %f\n", diff);
        fprintf(stderr, "--------------------------------------------\n");

    }
}

void test_bitpack ()
{
    uint64_t unsigned_word = (uint64_t)generate_num(0, 64);
    unsigned width = (unsigned)generate_num(1,63);

    assert(bitpack_fitsu(unsigned_word, width) == true);

    int64_t unsigned_word = (uint64_t)generate_num(0, 64);
    unsigned width = (uint64_t)generate_num(1,63);
    assert(bitpack_fitss(signed_word, width) == true);
}


int generate_num(int bound1, int bound2) 
{
     return (rand() % 256) +  bound1;

}

