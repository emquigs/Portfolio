/*
 * dct_module.h
 * Sam Broner Emily Quigley, 10/14/14
 *
 * Handles the conversion between blocks of 4 video component structs and the
 * structs that contain the discrete cosine transformation components of those
 * pixels (4 to 1).
 *
 *
*/


#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "arith40.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef SMALL_VIDEO_STRUCT
#define SMALL_VIDEO_STRUCT

typedef struct small_video {
        float a;
        float b;
        float c;
        float d;
        unsigned indexPb;
        unsigned indexPr;
}* small_video;

#endif

#ifndef VIDEO_STRUCT
#define VIDEO_STRUCT

typedef struct video_struct{
        float Y;
        float Pb;
        float Pr;
}* video_struct;

#endif



#ifndef DCT_MODULE_INCLUDED
#define DCT_MODULE_INCLUDED


extern A2Methods_UArray2 video_to_small_video(A2Methods_UArray2 video_array, 
                                              A2Methods_T methods_struct);

void block_condense (int i, int j, A2Methods_UArray2 rgb_array, 
                     A2Methods_Object *elem, void *cl);

extern A2Methods_UArray2 small_video_to_video(A2Methods_UArray2 
                                              small_video_array, 
                                              A2Methods_T methods_struct);

void block_expand(int i, int j, A2Methods_UArray2 rgb_array, 
                  A2Methods_Object *elem, void *cl);

float bounds_check(float var);

#endif