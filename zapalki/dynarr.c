#include "dynarr.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int addNumToDynArr(dynArr *darr, int64_t n)
{
    if(!darr) return 1;

    if(darr->n_emptySlots == 0) {
        darr->arr = (int64_t*)realloc(darr->arr, (darr->len + DYNARR_REALLOC_AT_ONCE) * sizeof(dynArr));
        if (!darr->arr) {
            fprintf(stderr, "ERROR: realloc() for dynarr->arr failed!");
            destroyDynArr(darr);
            return 2;
        }
        memset(&darr->arr[darr->n_allocatedSlots], 0, DYNARR_REALLOC_AT_ONCE * sizeof(int64_t));
        darr->n_allocatedSlots += DYNARR_REALLOC_AT_ONCE;
        darr->n_emptySlots = DYNARR_REALLOC_AT_ONCE;
    } else {
        darr->n_emptySlots--;
    }

    darr->arr[darr->len] = n;
    darr->len++;

    return 0;
}

dynArr *createDynArr()
{
    dynArr *darr = (dynArr*)malloc(sizeof(dynArr));
    if (!darr) return NULL;

    darr->len = 0;
    darr->arr = (int64_t*)calloc(sizeof(int64_t), DYNARR_REALLOC_AT_ONCE);
    if (!darr->arr) {
        free(darr);
        return NULL;
    }
    darr->n_allocatedSlots = DYNARR_REALLOC_AT_ONCE;
    darr->n_emptySlots = DYNARR_REALLOC_AT_ONCE;

    return darr;
}

void destroyDynArr(dynArr *darr)
{
    if (!darr) return;

    if (darr->arr) free(darr->arr);
    free(darr);
}
 
dynArr *mergeDynArrs(dynArr *darr1, dynArr *darr2)
{
    uint32_t totalLength = darr1->len + darr2->len;
    size_t newSize = totalLength / DYNARR_REALLOC_AT_ONCE + (totalLength % DYNARR_REALLOC_AT_ONCE != 0) * DYNARR_REALLOC_AT_ONCE;
    if (newSize > darr1->len) {
        darr1->arr = (int64_t*)realloc(darr1->arr, newSize);
        if (darr1->arr == NULL) {
            destroyDynArr(darr1);
            destroyDynArr(darr2);
            return NULL;
        }
    }

    memcpy(&darr1->arr[darr1->len], darr2->arr, darr2->len * sizeof(int64_t));

    memset(&darr1->arr[darr1->len + darr2->len], 0, (newSize - totalLength) * sizeof(int64_t));

    darr1->n_allocatedSlots = newSize;
    darr1->n_emptySlots = newSize - totalLength;
    darr1->len = totalLength;

    destroyDynArr(darr2);
    return darr1;
}
