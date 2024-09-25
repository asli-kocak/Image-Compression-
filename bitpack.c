/*
 * BITPACK.C
 *
 * Created by: Skylar Gilfeather, Asli Kocak
 * On: March 6, 2022
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include <cassert.h>
#include <except.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

#define zero (uint64_t)0;
#define ones (~(uint64_t)0);

static int pow2(int exponent);

/* Bitpack
 * Bitpack_fitsu()
 *
 * Returns a std bool designating whether an unsigned integer, n, can be
 * represented in width bits.
 *
 * @param uint64_t n        unsigned value to check width of
 * @param unsigned width    number of bits to fit n within
 *
 * @returns	a std bool: true if n can be represented in width bits, and
 *                      false otherwise
 * 
 * @expects             none.
 *
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    int upper = pow2(width) - 1; 
    return (n <= upper) ? true : false; 
}

/* Bitpack
 * Bitpack_fitss()
 *
 * Returns a std bool designating whether a signed integer, n, can be
 * represented in width bits.
 *
 * @param uint64_t n        signed value to check width of
 * @param unsigned width    number of bits to fit n within
 *
 * @returns	a std bool: true if n can be represented in width bits, and
 *                      false otherwise
 * 
 * @expects             none; however, width must be greater or equal to 0
 *                      to be passed into pow2(). 
 *
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    int upper = pow2(width - 1) - 1;
    int lower = (-1 * pow2(width - 1));
    return (n <= upper && n >= lower) ? true : false; 
}

/* Bitpack
 * Bitpack_getu()
 *
 * Given an unsigned 64-bit word, retrieves a field of length "width" bits
 * starting at index "lsb" (least sig. bit) within the word and returns it.  
 *
 * @param uint64_t word     unsigned word to read field value from 
 * @param unsigned width    number of bits which field value spans
 * @param unsigned lsb      index of field value's least significant bit
 *                          within word
 *
 * @returns	an unsigned 64-bit int containing the field value read.
 * 
 * @expects             width is greater than 0 and less than 64
 *                      lsb is greater than 0 and less than 64
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert (width > 0 && width < 64);
    assert (lsb >= 0  && width + lsb <= 64);

    uint64_t mask   = (ones >> (64 - width)) << lsb;
    uint64_t result = word & mask;
    result = result >> lsb;

    return result;
}

/* Bitpack
 * Bitpack_gets()
 *
 * Given an unsigned 64-bit word, retrieves a field of length "width" bits
 * starting at index "lsb" (least sig. bit) within the word and returns it.  
 *
 * @param uint64_t word     unsigned word to read field value from 
 * @param unsigned width    number of bits which field value spans
 * @param unsigned lsb      index of field value's least significant bit
 *                          within word
 *
 * @returns	a signed 64-bit int containing the field value read.
 * 
 * @expects             width is greater than 0 and less than 64
 *                      lsb is greater than 0 and less than 64
 */
 //
 //Bitpack_getu(0x3f4, 6, 2) == 61
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{

    /* check if signed is positive or negative */
    uint64_t check_sign_mask = ones << (width + lsb - 1);
    uint64 unsigned_result = Bitpack_getu(word, width - 1,  lsb);

    if (word & check_sign_mask == 0) {
        /* get value, avoiding the sign bit */
        return (int64_t) result; 
    }
    else {
        /* re-populate upper unused bits with ones */
        int msb = (width + lsb - 1);
        int64_t signed_result = unsigned_result; /* still has upper zeros */
        signed_result = signed_result << (64 - msb - 1);
        signed_result = signed_result >> (64 - msb - 1);

        return signed_result;
        /* if negative, convert to positive 
         * put into as unsigned funct
         * convert back to unsigned (add signed bit and take comp)
         */
    }
    assert(0);
    return zero;
}

/* Bitpack
 * Bitpack_newu()
 *
 * Given an unsigned 64-bit word and a unsigned value of "width" bits
 * overwrite the value into the word, starting at index "lsb" over width bits
 * in word. This returns the newly modified unsigned 64-bit word.
 *
 * @param uint64_t word     unsigned word to read field value from 
 * @param unsigned width    number of bits which field value spans
 * @param unsigned lsb      index of field value's least significant bit
 *                          within word
 * @param uint64_t value    the 64 bit value the client wants to write into
 *                          the word
 *
 * @returns	an unsigned 64-bit word containing the newly modified bits.
 * 
 * @expects             width is greater than 0 and less than 64
 *                      lsb is greater than 0 and less than 64
 *                      value is an unsigned integer that can fit
 *                      into "width" bits
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
    assert (width > 0 && width < 64);
    assert (lsb >= 0  && width + lsb <= 64);
    
    if (Bitpack_fitsu(value, width) == false) {
        RAISE(Bitpack_Overflow);
    }

    (void) word;
    (void) width;
    (void) lsb;
    (void) value;
    return 0;   
}


/* Bitpack
 * Bitpack_news()
 *
 * Given a 64 bit word and a signed value, will overwrite the value (also a 
 * 64 bit word) into the word.  
 *
 * @param uint64_t word     signed word to write the value in
 * @param unsigned width    number of bits which field value spans
 * @param unsigned lsb      index of least significant bit
 *                          within word
 * @param int64_t value     the signed 64 bit value the client wants to write
 *                          into the word
 *
 * @returns	a signed 64-bit int containing the field value read.
 * 
 * @expects             width is greater than 0 and less than 64
 *                      lsb is greater than 0 and less than 64
 *                      value is a signed integer of (width) bits that will
 *                      fit into the word
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
    assert (width > 0 && width < 64);
    assert (lsb >= 0  && width + lsb <= 64);

    if (Bitpack_fitss(value, width) == false) {
        RAISE(Bitpack_Overflow);
    }

    (void) word;
    (void) width;
    (void) lsb;
    (void) value;
    return 0;
}

/*
 * pow2()
 *
 * A recursive helper function, which computes 2^(exponent) and returns it.
 * 
 * @param int exponent      The exponent to compute the power of 2 for
 * 
 * @returns                 2^(exponent)
 * 
 * @expects                 int exponent is greater than or equal to 0
 * 
 */ 
static int pow2(int exponent) {

    if (exponent <= 0) {
        return 1;
    }
    else { 
        return 2 * pow2(exponent - 1);
    }
}