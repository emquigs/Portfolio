/*
 * compress40.c
 * Sam Broner Emily Quigley, 10/14/14
 *
 * Reads in a file and outsources compression or decompression logic to the 
 * modules: rgb_video, dct_module, bitpack_handler, and bitpack. Produces a
 * compressed image in binary compressed format and a decompressed image in a
 * ppm file written to standard output.
 *
*/


#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "pnm.h"
#include "bitpack.h"
#include "a2methods.h"
#include "a2blocked.h"
#include "uarray2b.h"
#include "dct_module.h"
#include "rgb_video.h"
#include "bitpack_handler.h"
#include "stdint.h"

static A2Methods_T methods;


/* compress40()
 * Takes in a file pointer to a .ppm image
 * Creates and frees the arrays needed by the 4 other compression modules used
 * by compress40 and calls their functions in the correct order to compress
 * the given image
 * Prints the binary compressed image
 */
void compress40(FILE *input)
{
        methods = uarray2_methods_blocked;

        Pnm_ppm input_img = Pnm_ppmread(input, methods);
        A2Methods_UArray2 video_img = trim_rgb_array_to_video_array(input_img, 
                                                                    methods);
        Pnm_ppmfree(&input_img);
        A2Methods_UArray2 small_video_img = video_to_small_video(video_img, 
                                                                 methods);
        methods->free(&video_img);
        A2Methods_UArray2 compressed_img = pack_codewords(small_video_img, 
                                                       methods);
        methods->free(&small_video_img);

        print_packed_elems(compressed_img);
        methods->free(&compressed_img);
}

/* decompress40()
 * Takes in a file pointer to a binary compressed image
 * Creates and frees the arrays needed by the 4 other decompression modules 
 * used by compress40 and calls their functions in the correct order to
 * decompress the given file into a ppm image
 * Writes the new ppm image to standard output
 */
void decompress40(FILE *input)
{
        methods = uarray2_methods_blocked;

        A2Methods_UArray2 compressed_array = read_packed_elems(input, methods);
        A2Methods_UArray2 small_video_img = unpack_codewords(compressed_array);
        methods->free(&compressed_array);
        A2Methods_UArray2 video_array = small_video_to_video(small_video_img, 
                                                             methods);
        methods->free(&small_video_img);
        Pnm_ppm decompressed_image = video_array_to_rgb_array(video_array, 
                                                              methods);
        methods->free(&video_array);

        Pnm_ppmwrite(stdout, decompressed_image);
        Pnm_ppmfree(&decompressed_image);
}