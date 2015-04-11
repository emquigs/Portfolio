/*
 * bitpack.c
 * Sam Broner Emily Quigley, 10/14/14
 *
 * bitpack performs bit manipulation and arithmetic, including size checking,
 * value insertion, and value retrieval
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "except.h"
#include "bitpack.h"
#include "assert.h"
#include "inttypes.h"

static uint64_t UZERO = (uint64_t)0;
static uint64_t UONE = (uint64_t)1;
static uint64_t UMAX = (uint64_t)64;

static int64_t SZERO = (uint64_t)0;
static int64_t SONE = (uint64_t)1;

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* shift_right()
 * Takes an unsigned word and an amount of spaces to shift  
 * Shifts the unsigned word right by the shift value
 * Fills the word with zeros from the left
 * returns the new word
 */
static inline uint64_t shift_right(uint64_t word, unsigned shift) {
        if (shift >= UMAX) {
                shift = shift % UMAX;
        }
        word = word >> shift;
        return word;
}

/* shift_left()
 * Takes an unsigned word and an amount of spaces to shift
 * Shifts the unsigned word left by the shift value
 * Returns the shifted word
 */
static inline uint64_t shift_left(uint64_t word, unsigned shift) {
        if (shift >= UMAX) {
                shift = shift % UMAX;
        }
        return(word << shift);
}

/* shift_right_s()
 * Takes in a signed word and an amount of places to shift
 * Shifts the signed word right by the shift value
 * Fills the word with 0's if it is a positive number or 1's if it is negative
 * Returns the shifted word
 */
static inline int64_t shift_right_s(int64_t word, unsigned shift) {

        if (word >= SZERO) {
                word = shift_right((uint64_t)word, shift);
                return word;
        } else {
                word = shift_right((uint64_t)word, shift) | 
                       shift_left(~UZERO, UMAX - shift);

                return word;
        }
}

/* Bitpack_fitsu()
 * Takes in a int64_t n and a width width
 * Checks if uint64_t n fits in width number of bits
 * returns 1 if yes, 0 if no
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (n < shift_left(UONE,width)) {
                return 1;
        }
        return 0;
}

/* Bitpack_fitss()
 * Takes in a int64_t n and a width width
 * Checks if int64_t n fits in width number of bits
 * returns 1 if yes, 0 if no
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        int64_t range = shift_left(UONE, width)/2;
        if (n <= ~(range - SONE) || n > range - SONE) {
                return 0;
        }
        return 1;
}

/* Bitpack_getyu()
 * Takes in a uint64_t word, a width, and a lsb
 * Gets the uint64_t stored from the lsb to the lsb plus width
 * Returns that uint64_t
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{

        assert(width <= UMAX);
        assert(width + lsb <= UMAX);
        assert(lsb >= UZERO);


        uint64_t mask = (shift_left(UONE, width) - UONE);
        word = shift_right(word, lsb);

        return mask & word;

}

/* Bitpack_gets()
 * Takes in a uint64_t word, a width, and a lsb
 * Gets the int64_t stored from the lsb to the lsb plus width
 * Returns that int64_t
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= UMAX);
        assert(width + lsb <= UMAX);
        assert(lsb >= UZERO);

        uint64_t mask = shift_left(UONE, width) - UONE;

        mask = shift_left(mask, lsb) & word;
        mask = shift_left(mask, UMAX - (lsb + width));

        mask = shift_right_s((int64_t)mask, UMAX - width);
        return mask;
}

/* Bitpack_newu()
 * Takes in a uint64_t word, a width, lsb, and and unsigned value
 * Value is inserted into the word at the given lsb
 * Returns updated word
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{

        if (!Bitpack_fitsu(value, width) || width > UMAX || (lsb < UZERO)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = UZERO;
        uint64_t clear_mask = shift_left(UONE, width) - UONE;

        clear_mask = shift_left(clear_mask, lsb);

        word = ~clear_mask & word;

        mask = mask + value;
        mask = shift_left(mask, lsb);
        return (word | mask);

}

/* Bitpack_news()
 * Takes in a uint64_t word, a width, lsb, and a signed value
 * Value is inserted into the word at the given lsb
 * Returns updated word
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                      int64_t value)
{

        if (!Bitpack_fitss(value, width) || width > UMAX || (lsb < UZERO)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = ~UZERO;
        uint64_t clear_mask = (shift_left(UONE, width) - UONE);

        clear_mask = shift_left(clear_mask, lsb);

        word = ~clear_mask & word;

        mask = shift_right(mask, UMAX - width) & value;
        return (word | shift_left(mask, lsb));

}
