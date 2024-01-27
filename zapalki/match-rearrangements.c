#include "match-rearrangements.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

bool isElligibleForRearrangement(int64_t num, Rearrangement *rearrangement)
{
    return false;
}

static int addRearrangement(RearrangementArray *rarr, uint8_t originBefore, uint8_t targetBefore, uint8_t originAfter, uint8_t targetAfter);

RearrangementArray *calculatePossibleRearrangements()
{
    RearrangementArray *rarr = (RearrangementArray*)malloc(sizeof(RearrangementArray));
    rarr->ptr = (Rearrangement*)malloc(sizeof(Rearrangement));
    Rearrangement *r = rarr->ptr;
    if (!r) return NULL;
    
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
                    if (newOriginNum > 0 && newTargetNum > 0) {
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
        if (rarr->ptr[i].targetBefore == NOT_A_NUMBER) {
            printf("%u -> %u; delta %i;\n", rarr->ptr[i].originBefore, rarr->ptr[i].originAfter, rarr->ptr[i].delta);
        } else {
            printf("%u, %u -> %u, %u; delta %i;\n", rarr->ptr[i].originBefore, rarr->ptr[i].targetBefore, rarr->ptr[i].originAfter, rarr->ptr[i].targetAfter, rarr->ptr[i].delta);
        }
    }
    printf("===== END REARRANGEMENT ARRAY =====\n");
}
#endif

static int addRearrangement(RearrangementArray *rarr, uint8_t originBefore, uint8_t targetBefore, uint8_t originAfter, uint8_t targetAfter)
{
    rarr->ptr = (Rearrangement*)realloc(rarr->ptr, (rarr->len + 1) * sizeof(Rearrangement));
    if (!rarr->ptr) return 1;

    Rearrangement *r = &rarr->ptr[rarr->len];

    if (originBefore == targetBefore) {
        r->originBefore = originBefore;
        r->targetBefore = NOT_A_NUMBER;
        r->originAfter = originAfter;
        r->targetAfter = NOT_A_NUMBER;
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
