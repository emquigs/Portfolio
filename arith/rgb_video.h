/*
 * rgb_video.h
 * Sam Broner Emily Quigley, 10/14/14
 *
 * Handles the conversion between rgb format and video component format, as
 * well as trimming files during compression
 *
 */

#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "pnm.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#ifndef VIDEO_STRUCT
#define VIDEO_STRUCT

typedef struct video_struct{
        float Y;
        float Pb;
        float Pr;
}* video_struct;

#endif

#ifndef RGB_VIDEO_INCLUDED
#define RGB_VIDEO_INCLUDED

extern A2Methods_UArray2 trim_rgb_array_to_video_array(Pnm_ppm input_img, 
                                                   A2Methods_T methods_struct);
struct video_struct rgb_to_video_component(Pnm_rgb rgb_struct, 
                                           int denominator);

extern Pnm_ppm video_array_to_rgb_array(A2Methods_UArray2 video_array, 
                                                   A2Methods_T methods_struct);
struct Pnm_rgb video_component_to_rgb(video_struct video_pixel);

#endif