/*
 * rgb_video.c
 * Sam Broner Emily Quigley, 10/14/14
 *
 * Handles the conversion between rgb format and video component format, as
 * well as trimming files during compression
 *
*/



#include "rgb_video.h"

static int DENOMINATOR = 255;

static A2Methods_T methods;

/* trim_rgb_array_to_video_array()
 * Takes in an image in PPM format and a methods object
 * Creates a new array of video structs with trimmed dimensions (if necessary)
 * and calls function that converts rgb structs into video structs on each
 * element in the 2D rgb struct array in the ppm
 * Returns new 2D array of video structs
 */
A2Methods_UArray2 trim_rgb_array_to_video_array(Pnm_ppm input_img, 
                                                A2Methods_T methods_struct)
{
        A2Methods_UArray2 new_array;
        methods = methods_struct;
        A2Methods_UArray2 orig_array = input_img->pixels;
        int width;
        int height;

        if (input_img->width %2) {
                input_img->width = (input_img->width) - 1;
        } 
        if (input_img->height %2) {
                input_img->height = (input_img->height) - 1;
        }

        width = input_img->width;
        height = input_img->height;

        new_array = methods->new_with_blocksize(width, height, 
                                                sizeof(struct video_struct), 
                                                2);
        
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {

                        video_struct new_ptr = 
                        (video_struct) methods->at(new_array, col, row);

                        *new_ptr = rgb_to_video_component(methods->at(
                                orig_array, col, row), input_img->denominator);
                        
                }
        }
        return new_array;
}

/* rgb_to_video_component()
 * Takes in an rgb struct and the denominator of a ppm image
 * Translates given rgb struct into a new video component struct with Y, Pb, &
 * Pr components
 * Returns the video struct
 */
struct video_struct rgb_to_video_component(Pnm_rgb rgb_struct, int denominator)
{
        float red = (float)rgb_struct->red/(float)denominator;
        float green = (float)rgb_struct->green/(float)denominator;
        float blue = (float)rgb_struct->blue/(float)denominator;

        struct video_struct video_pix;

        video_pix.Y = 0.299 * red + 0.587 * green + 0.114 * blue;
        video_pix.Pb = -0.168736 * red - 0.331264 * green + 0.5 * blue;
        video_pix.Pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;
        
        return video_pix;
}

/* video_array_to_rgb_array()
 * Takes in a 2D array of video structs and a methods object
 * Creates a new array of rgb structs and a new PPM calls function that 
 * converts video structs into rgb structs on each element in the 2D video 
 * struct array, then assigns the pixel array in the new ppm to this new array
 * of rgb structs
 * Returns new PPM image
 */
Pnm_ppm video_array_to_rgb_array(A2Methods_UArray2 video_array, 
                                 A2Methods_T methods_struct)
{

        methods = methods_struct;
        Pnm_ppm output_img = malloc(sizeof (*output_img));

        A2Methods_UArray2 new_array;

        int width = methods->width(video_array);
        int height = methods->height(video_array);

        output_img->width = width;
        output_img->height = height;
        output_img->denominator = DENOMINATOR;
        output_img->methods = methods;

        new_array = methods->new(width, height, sizeof(struct Pnm_rgb));

        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        Pnm_rgb new_ptr = 
                        (Pnm_rgb)methods->at(new_array, col, row);

                        *new_ptr = 
                        video_component_to_rgb(methods->at(video_array,
                                                           col, row));
                }
        }
        output_img->pixels = new_array;

        return output_img;
}
/* video_component_to_rgb()
 * Takes in an video struct
 * Translates given video struct into a new rgb component struct with red,
 * green, and blue components
 * Returns the rgb struct
 */
struct Pnm_rgb video_component_to_rgb(video_struct video_pixel)
{

        struct Pnm_rgb rgb_struct;

        float Y = video_pixel->Y;
        float Pb = video_pixel->Pb;
        float Pr = video_pixel->Pr;

        float red = (1.0 * Y + 0 * Pb + 1.402 * Pr) * DENOMINATOR;
        float green = (1.0 * Y - 0.344136 * Pb - 0.714136 * Pr) * DENOMINATOR;
        float blue = (1.0 * Y + 1.772 * Pb + 0.0 * Pr) * DENOMINATOR;

        if ( red < 0) {
                rgb_struct.red = 0;
        } else if (red > DENOMINATOR) { 
                rgb_struct.red = DENOMINATOR;
        } else {
                rgb_struct.red = (unsigned)red;
        }

        if (green < 0) {
                rgb_struct.green = 0;
        } else if (green > DENOMINATOR) {
                rgb_struct.green = DENOMINATOR;
        } else {
                rgb_struct.green = (unsigned)(green);
        }


        if (blue < 0) {
                rgb_struct.blue = 0;
        } else if (blue > DENOMINATOR) {
                rgb_struct.blue = DENOMINATOR;
        } else {
                rgb_struct.blue = (unsigned)(blue);
        }

        return rgb_struct;
}