/*
 * bitpack_handler.c
 * Sam Broner Emily Quigley, 10/14/14
 *
 * bitpack_handler deals with the setting and getting of bit-manipulated
 * codewords produced by the bitpack.h interface as well as reading in/printing
 * out compressed/decompressed images
 *
 */

#include "bitpack_handler.h"

static A2Methods_T methods;

static unsigned aw = 6;
static unsigned bw = 6;
static unsigned cw = 6;
static unsigned dw = 6;
static unsigned pbw = 4;
static unsigned prw = 4;
static unsigned alsb = 26;
static unsigned blsb = 20;
static unsigned clsb = 14;
static unsigned dlsb = 8;
static unsigned pblsb = 4;
static unsigned prlsb = 0;
static float unsigned_factor = 63;
static float signed_factor = 103.3333;

/* pack_blocks()
 * Takes in a 2D array of small video structs and a methods object
 * Creates array of bitpacked elements, calls map fn that will convert
 * the small video structs into bitpacked elements
 * Returns new compressed 2D array
 */

A2Methods_UArray2 pack_codewords (A2Methods_UArray2 small_video_array, 
                               A2Methods_T methods_struct)
{
        methods = methods_struct;
        A2Methods_UArray2 compressed_array;
        compressed_array = methods->new(methods->width(small_video_array), 
                           methods->height(small_video_array), 
                           sizeof(uint64_t));

        methods->map_default(small_video_array, pack_elem, &compressed_array);
        return compressed_array;
}

/* pack_elem()
 * Takes in location of elem, 2D array of small video structs, a pointer to
   an elem, and a closure (which is the compressed array to be updated)
 * Converts the small video struct elem into bitpacked elem using bitpack.h
 * interface and puts it into next open spot in compressed array
 * Returns nothing
 */
void pack_elem(int i, int j, A2Methods_UArray2 small_video_array, 
               A2Methods_Object *ptr, void *cl)
{
        (void) small_video_array;
        small_video elem = (small_video)ptr;

        unsigned a = (unsigned)(elem->a * unsigned_factor);
        signed b = (signed) (elem->b * signed_factor);
        signed c = (signed) (elem->c  * signed_factor);
        signed d = (signed) (elem->d * signed_factor);
        unsigned Pb = elem->indexPb;
        unsigned Pr = elem->indexPr;

        uint64_t codeword = 0;

        codeword = Bitpack_newu(codeword, prw, prlsb, Pr);
        codeword = Bitpack_newu(codeword, pbw, pblsb, Pb);
        codeword = Bitpack_news(codeword, dw, dlsb, d);
        codeword = Bitpack_news(codeword, cw, clsb, c);
        codeword = Bitpack_news(codeword, bw, blsb, b);
        codeword = Bitpack_newu(codeword, aw, alsb, a);

        *(uint64_t *)methods->at(*(A2Methods_UArray2 *)cl, i, j) = codeword;
}
/* print_packed_elems()
 * Takes in a 2D array of bitpacked elements
 * Prints the compressed image to standard output
 * Returns nothing
 */
void print_packed_elems (A2Methods_UArray2 compressed_img)
{
        (void) compressed_img;

        int chars_in_cword = ((aw + bw + cw + dw + pbw + prw)/8) - 1;


        int width = methods->width(compressed_img);
        int height = methods->height(compressed_img);
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);

        char c;
        for (int row = 0; row < height; row++) {
                for (int col = 0; col < width; col++) {
                        for (int byte_cnt = chars_in_cword; byte_cnt >= 0; 
                                                byte_cnt--){
                                c = Bitpack_getu(*(uint64_t *)methods->at(
                                                    compressed_img, col, row), 
                                                    8, byte_cnt * 8);

                                putchar(c);
                        }
                }
        }
}

/* read_packed_elems()
 * Takes in a file pointer to a file of a binary compressed image and a methods
 * object
 * Creates array of bitpacked elements and inserts compressed elements from 
 * given file into the array
 * Returns new compressed 2D array
 */

A2Methods_UArray2 read_packed_elems(FILE *fp, A2Methods_T methods_struct)
{
        methods = methods_struct;

        unsigned height, width;
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        uint64_t codeword;
        assert(methods);

        A2Methods_UArray2 compressed_img = methods->new(width, height, 
                                                        sizeof(uint64_t));
        assert(compressed_img);

        int chars_in_cword = ((aw + bw + cw + dw + pbw + prw)/8) - 1;

        for (int row = 0; row < (int)height; row++) {
                for (int col = 0; col < (int)width; col++) {
                        for (int byte_cnt = chars_in_cword; byte_cnt >= 0; 
                                                                byte_cnt--){
                                
                                c = getc(fp);
                                codeword = Bitpack_newu(codeword, 8, 
                                                        byte_cnt * 8, c);
                        } 
                        *(uint64_t *)methods->at(compressed_img, col, row) = 
                        codeword;
                }
        }
        return compressed_img;
}

/* unpack_blocks()
 * Takes in a 2D array of bitpack-compressed elements
 * Creates array of small video structs and calls map function that will 
 * unpack every element of the given array using bitpack.h
 * Returns new 2D array of small video structs
 */

A2Methods_UArray2 unpack_codewords (A2Methods_UArray2 compressed_array)
{
        A2Methods_UArray2 small_video_array;
        small_video_array = 
        methods->new(methods->width(compressed_array), 
                     methods->height(compressed_array), 
                     sizeof (struct small_video));

        methods->map_default(compressed_array, unpack_elem, 
                             &small_video_array);

        return small_video_array;
}

/* unpack_elem()
 * Takes in location of elem, 2D array of bitpacked uint64_t codewords, 
 * a pointer to the elem, and a closure (which is the small video array to be 
 * updated)
 * Unpacks element using bitpack.h and puts individual values into a small 
 * video struct that is then inserted into the next available location in the
 * closure array
 * Returns nothing
 */

void unpack_elem(int i, int j, A2Methods_UArray2 compressed_array, 
                 A2Methods_Object *ptr, void *cl)
{
        
        (void) compressed_array;
        struct small_video element;

        element.indexPr = Bitpack_getu(*(uint64_t *)ptr, prw, prlsb);
        element.indexPb = Bitpack_getu(*(uint64_t *)ptr, pbw, pblsb);
        element.d = ((float) (Bitpack_gets(*(uint64_t *)ptr, dw, dlsb) / 
                     signed_factor));
        element.c = ((float) (Bitpack_gets(*(uint64_t *)ptr, cw, clsb) / 
                     signed_factor));
        element.b = ((float) (Bitpack_gets(*(uint64_t *)ptr, bw, blsb) / 
                     signed_factor));
        element.a = ((float) (Bitpack_getu(*(uint64_t *)ptr, aw, alsb) / 
                     unsigned_factor));


        *(small_video)methods->at(*(A2Methods_UArray2 *)cl, i, j) = element;
}