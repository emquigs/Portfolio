/*
 * bitpack_handler.h
 * Sam Broner Emily Quigley, 10/14/14
 *
 * bitpack_handler deals with the setting and getting of bit-manipulated
 * codewords produced by the bitpack.h interface as well as reading in/printing
 * out compressed/decompressed images
 *
 */

#include "stdint.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "bitpack.h"
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

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

#ifndef BITPACK_HANDLER_INCLUDED
#define BITPACK_HANDLER_INCLUDED

extern A2Methods_UArray2 pack_codewords (A2Methods_UArray2 small_video_array, 
                                      A2Methods_T methods_struct);
void pack_elem(int i, int j, A2Methods_UArray2 small_video_array, 
               A2Methods_Object *ptr, void *cl);

extern void print_packed_elems (A2Methods_UArray2 compressed_img);

extern A2Methods_UArray2 read_packed_elems(FILE *fp, 
                                           A2Methods_T methods_struct);

extern void unpack_elem(int i, int j, A2Methods_UArray2 small_video_array, 
                        A2Methods_Object *ptr, void *cl);
A2Methods_UArray2 unpack_codewords (A2Methods_UArray2 compressed_array);

#endif