#include "dynarr.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int addNumToDynArr(dynArr *darr, int64_t n)
{
    if(!darr) return 1;

    if(darr->n_emptySlots == 0) {
        darr->arr = realloc(darr->arr, (darr->len + DYNARR_REALLOC_AT_ONCE) * sizeof(dynArr));
        if (!darr->arr) {
            fprintf(stderr, "ERROR: realloc() for dynarr->arr failed!");
            destroyDynArr(darr);
            return 2;
        }
        memset(&darr->arr[darr->n_allocatedSlots - 1], 0, DYNARR_REALLOC_AT_ONCE * sizeof(int64_t));
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
    dynArr *darr = malloc(sizeof(dynArr));
    if (!darr) return NULL;

    darr->len = 0;
    darr->arr = calloc(sizeof(int64_t), DYNARR_REALLOC_AT_ONCE);
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

    free(darr->arr);
    free(darr);
}
