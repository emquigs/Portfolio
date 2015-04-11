/*
 * dct_module.c
 * Sam Broner Emily Quigley, 10/14/14
 *
 * Handles the conversion between blocks of 4 video component structs and the
 * structs that contain the discrete cosine transformation components of those
 * pixels (4 to 1).
 *
*/


#include "dct_module.h"

static A2Methods_T methods;

typedef struct block_cl {
        int counter;
        A2Methods_UArray2 new_array;
}* block_cl;


/* video_to_small_video()
 * Takes in a 2D array of video structs and a methods object
 * Creates array of small video structs, calls map fn that will combine the
 * video structs in a 4-element block into one small video struct
 * Returns new 2D array of small video structs
 */
A2Methods_UArray2 video_to_small_video(A2Methods_UArray2 video_array, 
                                                    A2Methods_T methods_struct)
{
        int counter = 0;
        block_cl my_cl = malloc(sizeof *my_cl);
        methods = methods_struct;

        A2Methods_UArray2 new_array = 
                                methods->new(methods->width(video_array)/2, 
                                            methods->height(video_array)/2, 
                                            sizeof(struct small_video));
        my_cl->counter = counter;
        my_cl->new_array = new_array;

        methods->map_default(video_array, block_condense, my_cl);

        free(my_cl);

        return new_array;
}
/* block_condense()
 * Takes in a 2D array of video structs, an element's location, a pointer to
 * the element, and a closure that contains a new array of small video structs
 * Combines the video structs in a 4-element block into one small-video-struct
 * and inserts it into the next empty location in the small video array
 * Returns nothing
 */
void block_condense (int i, int j, A2Methods_UArray2 video_array, 
                                            A2Methods_Object *elem, void *cl)
{
        (void) elem;
        A2Methods_UArray2 condensed_array = ((block_cl)cl)->new_array;
        if(((block_cl)cl)->counter == 3) {
                ((block_cl)cl)->counter = 0;

                video_struct p1, p2, p3, p4;

                small_video dct_pixel = malloc(sizeof(*dct_pixel));

                p4 = (video_struct)methods->at(video_array, i, j);
                p3 = (video_struct)methods->at(video_array, i-1, j);
                p2 = (video_struct)methods->at(video_array, i, j-1);
                p1 = (video_struct)methods->at(video_array, i-1, j-1);

                dct_pixel->a = (p4->Y + p3->Y + p2->Y + p1->Y)/4;
                dct_pixel->b = bounds_check((p4->Y + p3->Y - p2->Y - p1->Y)/4);
                dct_pixel->c = bounds_check((p4->Y - p3->Y + p2->Y - p1->Y)/4);
                dct_pixel->d = bounds_check((p4->Y - p3->Y - p2->Y + p1->Y)/4);

                dct_pixel->indexPb = Arith40_index_of_chroma(
                                        (p4->Pb + p3->Pb + p2->Pb + p1->Pb)/4);

                dct_pixel->indexPr = Arith40_index_of_chroma(
                                        (p4->Pr + p3->Pr + p2->Pr + p1->Pr)/4);

                *(small_video)methods->at(condensed_array, (i-1)/2, (j-1)/2) = 
                                                                    *dct_pixel;

                free(dct_pixel);
        } else {
                ((block_cl)cl)->counter = ((block_cl)cl)->counter + 1;
        }
}

/* bounds_check()
 * Takes in a floating point number
 * Checks to make sure it is in the range of -0.3 to 0.3
 * Returns 0.3 if the number is greater than 0.3, -0.3 if it is less than -0.3,
 * or the given number if it is within the bounds
 */

float bounds_check(float var)
{
        if (var > 0.3){
                return 0.3;
        } else if (var < -0.3) {
                return -0.3;
        } else {
                return var;
        }
}

/* small_video_to_video()
 * Takes in a 2D array of small video structs and a methods object
 * Creates array of video structs, calls map fn that will expand the
 * small video structs into a 4-element block of video structs
 * Returns new 2D array of video structs
 */
A2Methods_UArray2 small_video_to_video(A2Methods_UArray2 small_video_array, 
                                                    A2Methods_T methods_struct)
{
        methods = methods_struct;

        A2Methods_UArray2 new_array = 
                            methods->new(methods->width(small_video_array)*2, 
                                        methods->height(small_video_array)*2, 
                                        sizeof(struct video_struct));

        methods->map_default(small_video_array, block_expand, &new_array);

        return new_array;
}

/* block_expand()
 * Takes in a 2D array of small video structs, an element's location, a pointer
 * to the element, and a closure that contains a new array of video structs
 * Expands the small video structs into a 4-element block of video structs
 * Returns nothing
 */
void block_expand(int i, int j, A2Methods_UArray2 small_video_array, 
                                            A2Methods_Object *elem, void *cl)
{
        (void)small_video_array;
        A2Methods_UArray2 video_array = *(A2Methods_UArray2 *)cl;

        struct video_struct p1;
        struct video_struct p2;
        struct video_struct p3;
        struct video_struct p4;

        float a = ((small_video)elem)->a;
        float b = ((small_video)elem)->b;
        float c = ((small_video)elem)->c;
        float d = ((small_video)elem)->d;
        float Pb = Arith40_chroma_of_index(((small_video)elem)->indexPb);
        float Pr = Arith40_chroma_of_index(((small_video)elem)->indexPr);

        p1.Y = a - b - c + d;
        p2.Y = a - b + c - d;
        p3.Y = a + b - c - d;
        p4.Y = a + b + c + d;

        p1.Pb = Pb;
        p2.Pb = Pb;
        p3.Pb = Pb;
        p4.Pb = Pb;

        p1.Pr = Pr;
        p2.Pr = Pr;
        p3.Pr = Pr;
        p4.Pr = Pr;

        *(video_struct)methods->at(video_array, i*2, j*2) = p1;
        *(video_struct)methods->at(video_array, i*2 + 1, j*2) = p2;
        *(video_struct)methods->at(video_array, i*2, j*2 + 1) = p3;
        *(video_struct)methods->at(video_array, i*2 + 1, j*2 + 1) = p4;
}
