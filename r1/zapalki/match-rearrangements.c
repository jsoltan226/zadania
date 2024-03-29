#include "match-rearrangements.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

bool isElligibleForRearrangement(int64_t num1, int64_t num2, Rearrangement *rearrangement)
{
    if ((num1 == NOT_A_NUMBER_I64 && num2 == NOT_A_NUMBER_I64) || rearrangement == NULL) return false;

    char num1str[DIGIT_STR_BUF_SIZE] = { 0 };
    char num2str[DIGIT_STR_BUF_SIZE] = { 0 };
    snprintf(num1str, DIGIT_STR_BUF_SIZE - 1, "%li", num1);
    snprintf(num2str, DIGIT_STR_BUF_SIZE - 1, "%li", num2);

    char *c_ptr_1 = num1str;
    char *c_ptr_2 = num2str;
    do {
        if (*c_ptr_1 == '-' || !*c_ptr_1) continue;
        int8_t current_num1_digit = (*c_ptr_1) - 48;
        if (num2 == NOT_A_NUMBER_I64) {
            if (rearrangement->originBefore == current_num1_digit && rearrangement->targetBefore == NOT_A_NUMBER_I8)
                return true;
        }
        do {
            if (*c_ptr_2 == '-' || !*c_ptr_2) continue;
            int8_t current_num2_digit = (*c_ptr_2) - 48;

            if ((rearrangement->originBefore == current_num1_digit && rearrangement->targetBefore == current_num2_digit) || 
                (rearrangement->originBefore == current_num1_digit && rearrangement->targetBefore == NOT_A_NUMBER_I8 && num1 == NOT_A_NUMBER_I64)
                    ) {
                return true;
            }
        } while (*c_ptr_2++);
    } while (*c_ptr_1++);
    return false;
}

#ifdef DEBUG
static int addRearrangement(RearrangementArray *rarr, uint8_t originBefore, uint8_t targetBefore, uint8_t originAfter, uint8_t targetAfter);

RearrangementArray *calculatePossibleRearrangements()
{
    RearrangementArray *rarr = (RearrangementArray*)malloc(sizeof(RearrangementArray));
    rarr->ptr = (Rearrangement*)malloc(sizeof(Rearrangement));
    Rearrangement *r = rarr->ptr;
    if (!r) return NULL;
    
    /* This whole process goes as follows: 
     *  1. Pick a number from which the match will be moved, let's call it the 'origin';
     *  2. Pick a number to which the match will be moved, the 'target';
     *
     *  3. Pick a segment on the origin that has a match on it, call it the 'match_original'
     *  4. Pick a segment on the target that doesn't have a match on it, this will be the 'match_moved' one 
     *
     *  5. If 'origin' and 'target' are the same:
     *      5.1.1 Remove the match at 'match_original' and place one for 'match_moved' for both 'origin' and 'target'
     *      
     *  5.1 If 'origin' and 'target' are different:
     *      5.1.1 Place a match in the 'match_moved' segment of 'target' 
     *      5.1.2 Remove the match at 'match_original' in 'origin' 
     *
     *  6. Check if both 'origin' and 'target' are valid numbers
     *      If they are, the current rearrangement is valid. 
     *
     *  7. Repeat the process for all combinations of origins, targets and match displacements. */

    for (int i = 0; i < N_DIGITS; i++) {
        const m_Digit d_origin = matchLayouts[i];

        for (int j = 0; j < N_DIGITS; j++) {
            const m_Digit d_target = matchLayouts[j];

            for (m_Digit match_original = 1 << (N_MAX_MATCHES_PER_DIGIT - 1); match_original >= 1; match_original >>= 1) {
                if (!(d_origin & match_original)) continue;

                for (m_Digit match_moved = 1 << (N_MAX_MATCHES_PER_DIGIT - 1); match_moved >= 1; match_moved >>= 1) {
                    if (d_target & match_moved) continue;

                    m_Digit new_d_origin, new_d_target;
                    if (d_origin == d_target) {
                        new_d_origin = (d_target | match_moved) & (~match_original);
                        new_d_target = new_d_origin;
                    } else {
                        new_d_origin = d_origin & (~match_original);
                        new_d_target = d_target | match_moved;
                    }

                    int newOriginNum = -1, newTargetNum = -1;
                    for (int k = 0; k < N_DIGITS; k++) {
                        if (new_d_origin == matchLayouts[k]) newOriginNum = k;
                        if (new_d_target == matchLayouts[k]) newTargetNum = k;
                    }
                    if (newOriginNum >= 0 && newTargetNum >= 0) {
                        if (addRearrangement(rarr, i, j, newOriginNum, newTargetNum)) {
                            fprintf(stderr, "Failed to add %u, %u -> %u, %u rearrangement.\n", i, j, newOriginNum, newTargetNum);
                        }
                    }
                }
            }
        }
    }

    return rarr;
}
#endif

void destroyRearrangementArray(RearrangementArray *rarr)
{
    free(rarr->ptr);
    free(rarr);
}

#ifdef DEBUG
void printRearrangements(RearrangementArray *rarr)
{
    printf("===== BEGIN REARRANGEMENT ARRAY =====\n");
    for (int i = 0; i < rarr->len; i++) {
        if (rarr->ptr[i].targetBefore == NOT_A_NUMBER_I8 && rarr->ptr[i].targetAfter == NOT_A_NUMBER_I8) {
            printf("%u -> %u; delta %i;\n", rarr->ptr[i].originBefore, rarr->ptr[i].originAfter, rarr->ptr[i].delta);
        } else {
            printf("%u, %u -> %u, %u; delta %i;\n", rarr->ptr[i].originBefore, rarr->ptr[i].targetBefore, rarr->ptr[i].originAfter, rarr->ptr[i].targetAfter, rarr->ptr[i].delta);
        }
    }
    printf("===== END REARRANGEMENT ARRAY =====\n");
}

static int addRearrangement(RearrangementArray *rarr, uint8_t originBefore, uint8_t targetBefore, uint8_t originAfter, uint8_t targetAfter)
{
    rarr->ptr = (Rearrangement*)realloc(rarr->ptr, (rarr->len + 1) * sizeof(Rearrangement));
    if (!rarr->ptr) return 1;

    Rearrangement *r = &rarr->ptr[rarr->len];

    if (originBefore == targetBefore) {
        r->originBefore = originBefore;
        r->targetBefore = NOT_A_NUMBER_I8;
        r->originAfter = originAfter;
        r->targetAfter = NOT_A_NUMBER_I8;
        r->delta = originAfter - originBefore;
    } else {
        r->originBefore = originBefore;
        r->targetBefore = targetBefore;
        r->originAfter = originAfter;
        r->targetAfter = targetAfter;

        r->delta = (originAfter + targetAfter) - (originBefore + targetBefore);
    }

    rarr->len++;
    return 0;
}
#endif
