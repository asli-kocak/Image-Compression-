/*
 * VIDEO_PIXEL.H
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 2, 2022
 * 
 * 
 */

#ifndef VIDEO_PIX_INCLUDED
#define VIDEO_PIX_INCLUDED

#include <stdlib.h>
#include "pnm.h"


typedef struct Video_pixel {
    float y, pb, pr;
} Video_pixel;

Video_pixel convert_rgb_to_video(struct Pnm_rgb pixel, unsigned denom);

struct Pnm_rgb convert_video_to_rgb(Video_pixel pixel, unsigned denom);

#endif