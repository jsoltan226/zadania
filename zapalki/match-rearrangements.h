#ifndef REARRANGEMENTS_H_
#define REARRANGEMENTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define N_MAX_MATCHES_PER_DIGIT 7
#define N_DIGITS 10
#define NOT_A_NUMBER 255

typedef struct {
    uint8_t originBefore, targetBefore;
    uint8_t originAfter, targetAfter;
    int32_t delta;
} Rearrangement;

typedef struct {
    Rearrangement *ptr;
    size_t len;
} RearrangementArray;

typedef unsigned char m_Digit;

/*     
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
 *  Bit 8 is unused
 *  Bits are read from right to left!
*/
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

bool isElligibleForRearrangement(int64_t num, Rearrangement *rearrangement);
RearrangementArray *calculatePossibleRearrangements();
#ifdef DEBUG
void printRearrangements(RearrangementArray *rarr);
#endif
void destroyRearrangementArray(RearrangementArray *rarr);

#endif /* REARRANGEMENTS_H_ */
