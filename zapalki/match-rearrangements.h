#ifndef REARRANGEMENTS_H_
#define REARRANGEMENTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define N_DIGITS 10

typedef unsigned char m_Digit;
/* Like a 7-segment display, where each bit represents a segment.
 * This is used to represent a number in the format required by this challenge.
 * The bit being set to 1 indicates that a match is present in the given segment.
 *
 *        bit 1    
 *        =======
 * bit 2 |       | bit 3
 *    => |       | <=
 *       | bit 4 |
 *        =======
 *       |       |
 * bit 5 |       | bit 6
 *    => |       | <=
 *        =======
 *        bit 7 ^
 *
 *  Bit 8 is obviously unused
 *  Bits are read from right to left!
*/
#define N_MAX_MATCHES_PER_DIGIT 7

/* Pretty self explanatory */
static const m_Digit matchLayouts[N_DIGITS] = { 
    0b01110111, /* 0 */
    0b00100100, /* 1 */
    0b01011101, /* 2 */
    0b01101101, /* 3 */
    0b00101110, /* 4 */
    0b01101011, /* 5 */
    0b01111011, /* 6 */
    0b00100101, /* 7 */
    0b01111111, /* 8 */
    0b01101111  /* 9 */
};

/* A 'Rearrangement' is a struct that can hold data about a (by implication) valid way
 * to move a match from one digit to another.
 * It keeps track of how the digits looked like before and after the rearrangement,
 * as well as the difference between the 'before' and 'after' states. */
typedef struct {
    int8_t originBefore, targetBefore;
    int8_t originAfter, targetAfter;
    int16_t delta;
} Rearrangement;
#define NOT_A_NUMBER -1

/* Basically a half-baked std::vector<Rearrangement> */
typedef struct {
    Rearrangement *ptr;
    size_t len;
} RearrangementArray;


/* Currently not working */
bool isElligibleForRearrangement(int64_t num1, int64_t num2, Rearrangement *rearrangement);

#ifdef DEBUG
/* I think you can guess what this does... */
RearrangementArray *calculatePossibleRearrangements();

/* And this... */
void printRearrangements(RearrangementArray *rarr);
#endif

/* And also this. */
void destroyRearrangementArray(RearrangementArray *rarr);

#endif /* REARRANGEMENTS_H_ */
