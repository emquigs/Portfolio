#include "stdio.h"
#include "stdlib.h"
#include "bitpack.h"
#include "assert.h"

void check_laws(uint64_t word, unsigned w, unsigned lsb, uint64_t val, unsigned w2, unsigned lsb2);

int main(int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        FILE *randfp = fopen("/dev/urandom", "rb");
        assert(randfp);

        uint64_t word;
        uint64_t val;
        size_t rc;
        unsigned w2;
        unsigned lsb2;


        for (unsigned w = 0; w <= 64; w++) {
                for (unsigned lsb = 0; lsb + w <= 64; lsb++) {
                        for (unsigned trial = 0; trial < 10; trial++) {

                                rc = fread(&word, sizeof(word), 1, randfp);
                                assert(rc == 1);

                                rc = fread(&val, sizeof(val), 1, randfp);
                                assert(rc == 1);
                                
                                val = val >> (uint64_t)(64-w);


                                //the following two lines are garbage, fix them tomorrow
                                lsb2 = w + lsb; 
                                w2 = lsb;

                                check_laws(word, w, lsb, val, w2, lsb2);
                        }
                }
        }
}
void check_laws(uint64_t word, unsigned w, unsigned lsb, uint64_t val, unsigned w2, unsigned lsb2)
{
        TRY
                assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w, lsb) == val);

        ELSE
                fprintf(stderr, "Law 1 failed\n");
        END_TRY;

        TRY
                assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w2, lsb2) == Bitpack_getu(word, w2, lsb2));

        ELSE
                fprintf(stderr, "Law 2 failed\n");
        END_TRY;

        // TRY
        //         assert();

        // ELSE
        //         fprintf(stderr, "Law 3\n");
        // END_TRY;


}

// int main (int argc, char *argv[])
// {
//         (void) argc;
//         (void) argv;

//         unsigned a = 152;
//         signed b = 0;
//         signed c = 0;
//         signed d = 0;
//         unsigned Pb = 2;
//         unsigned Pr = 15;

//         uint64_t codeword = 0;

//         codeword = Bitpack_newu(codeword, 4, 0, Pr);
//         // printf("%ld\n", (Bitpack_getu(codeword, 4, 0)));

//         codeword = Bitpack_newu(codeword, 4, 4, Pb);
//         codeword = Bitpack_news(codeword, 5, 8, d);

//         printf("%ld\n", (Bitpack_gets(codeword, 5, 8)));

//         codeword = Bitpack_news(codeword, 5, 13, c);
//         codeword = Bitpack_news(codeword, 5, 18, b);
//         codeword = Bitpack_newu(codeword, 9, 23, a);

        

//         // int64_t a = 6;
//         // int64_t b = 3;

//         unsigned width = 6;
//         //unsigned lsb = 2;
//         //uint64_t valueu = 3;
//         int64_t values = -3;
//         //uint64_t valueu2 = 4;
//         //uint64_t values2 = -4;

//         //uint64_t word = 0x3f4;
//         //unsigned w2 = 8;
//         //unsigned lsb2 = lsb;

//         if(Bitpack_fitsu(a,b)) {
//                 printf("I can fitsu (%#"PRIx64", %#"PRIx64")!\n", a, b);
//         } else {
//                 printf("I can't fitsu (%#"PRIx64", %#"PRIx64")!\n", a,b);
//         }
//         if(Bitpack_fitss(a,b)) {
//                 printf("I can fitss (%#"PRIx64", %#"PRIx64")!\n", a, b);
//         } else {
//                 printf("I can't fitss (%#"PRIx64", %#"PRIx64")!\n", a,b);
//         }

//         TRY
//                 if (Bitpack_getu(Bitpack_newu(word, width, lsb, valueu), width, lsb) == valueu) {
//                         printf("I succeeded 1!\n");

//                 }
//         ELSE
//                 fprintf(stderr, "1 failed\n");
//         END_TRY;



//         TRY
//                 if (Bitpack_gets(Bitpack_news(word, width, lsb, values), width, lsb) == values) {
//                         printf("I succeeded 2!\n");

//                 }
//         ELSE
//                 fprintf(stderr, "2 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_getu(Bitpack_newu(word, width, lsb, valueu), w2, lsb2) == Bitpack_getu(word, w2, lsb2));
//                 printf("I succeeded 3!\n");

//         ELSE
//                 fprintf(stderr, "Law 3 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_gets(Bitpack_news(word, width, lsb, values), w2, lsb2) == Bitpack_gets(word, w2, lsb2));
//                 printf("I succeeded 4!\n");

//         ELSE
//                 fprintf(stderr, "Law 4 failed\n");
//         END_TRY;

//         TRY
//                 if (Bitpack_newu(Bitpack_newu(word, w2, lsb2, valueu2), width, lsb, valueu) == Bitpack_newu(Bitpack_newu(word, width, lsb, valueu), w2, lsb2, valueu2)){
//                         printf("I succeeded 5!\n");

//                 }

//         ELSE
//                 fprintf(stderr, "Law 5 failed\n");
//         END_TRY;

//         TRY
//                 if (Bitpack_news(Bitpack_news(word, w2, lsb2, values2), width, lsb, values) == Bitpack_news(Bitpack_news(word, width, lsb, values), w2, lsb2, values2)){
//                         printf("I succeeded 6!\n");
//                 }
//         ELSE
//                 fprintf(stderr, "Law 6 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_newu(Bitpack_newu(word, width, lsb, valueu), w2, lsb2, valueu2) == Bitpack_newu(word, w2, lsb2, valueu2));
//                 printf("I succeeded 7!\n");
//         ELSE
//                 fprintf(stderr, "Law 7 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_news(Bitpack_news(word, width, lsb, values), w2, lsb2, values2) == Bitpack_news(word, w2, lsb2, values2));
//                 printf("I succeeded 8!\n");
//         ELSE
//                 fprintf(stderr, "Law 8 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_getu(Bitpack_newu(word, width, lsb, valueu), UMAX - (lsb + width), lsb + width) == Bitpack_getu(word, UMAX - (lsb + width), lsb + width));
//                 printf("I succeeded 9!\n");

//         ELSE
//                 fprintf(stderr, "9 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_gets(Bitpack_news(word, width, lsb, values), UMAX - (lsb + width), lsb + width) == Bitpack_gets(word, UMAX - (lsb + width), lsb + width));
//                 printf("I succeeded 10!\n");
//         ELSE
//                 fprintf(stderr, "10 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_getu(Bitpack_newu(word, width, lsb, valueu), lsb, UZERO) == Bitpack_getu(word, lsb, UZERO));
//                 printf("I succeeded 11!\n");

//         ELSE
//                 fprintf(stderr, "11 failed\n");
//         END_TRY;

//         TRY
//                 assert(Bitpack_gets(Bitpack_news(word, width, lsb, values), lsb, UZERO) == Bitpack_gets(word, lsb, UZERO));
//                 printf("I succeeded 12!\n");
//         ELSE
//                 fprintf(stderr, "12 failed\n");
//         END_TRY;
//         if (Bitpack_fitss(values, width) == Bitpack_fitss(values << 2, width + 2)) {
//                 printf("1: True\n");
//         } else {
//                 printf("1: False\n");
//         }

//         if (Bitpack_fitss(values, width) == Bitpack_fitss(values >> 2, width - 2)) {
//                 printf("2: True\n");
//         } else {
//                 printf("2: False\n");
//         }


//         return 0;
        
// }
