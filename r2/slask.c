#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #include "dynarr.h" */
#ifndef DYNARR_H
#define DYNARR_H

/* Ummm.... WTF is an std::vector????????? */

#define DYNARR_REALLOC_AT_ONCE  100
typedef struct {
    uint64_t *arr;
    uint32_t len, n_allocatedSlots;
    uint32_t n_emptySlots;
} dynArr;
int addNumToDynArr(dynArr *darr, uint64_t n);
dynArr *createDynArr();
void destroyDynArr(dynArr *darr);
dynArr *mergeDynArrs(dynArr *darr1, dynArr *darr2);

#endif /* DYNARR_H */

static uint32_t query(uint32_t n_children_per_node, uint64_t nodeA, uint64_t nodeB);

int main(void)
{
    unsigned int n_iterations = 0;
    scanf("%u", &n_iterations);
    for (int i = 0; i < n_iterations; i++) {
        uint32_t N;
        uint64_t a, b;
        scanf("%u %lu %lu", &N, &a, &b);
        printf("%u\n", query(N, a, b));
    }
    return 0;
}

static uint32_t query(uint32_t N, uint64_t a, uint64_t b)
{
	//printf("a: %lu; b: %lu; N: %u\n", a, b, N);
    if (a == b) return a;
	if (N == 1) return a < b ? a : b;
    dynArr *subtreesA = NULL, *subtreesB = NULL;   
    subtreesA = createDynArr();
    subtreesB = createDynArr();
    
    uint64_t x = a;
    dynArr *subtreeArr = subtreesA;
    for (int i = 0; i < 2; i++) {
		do {
            addNumToDynArr(subtreeArr, x);
            x = x / N + (x % N > 1);
        } while (x > 0);
        x = b;
        subtreeArr = subtreesB;
    }

    uint32_t retIndex = 0;
    while (subtreesA->arr[subtreesA->len - 1 - retIndex] == subtreesB->arr[subtreesB->len - 1 - retIndex]) retIndex++;

    uint32_t retVal = subtreesA->arr[subtreesA->len - retIndex];
    destroyDynArr(subtreesA);
    destroyDynArr(subtreesB);
    return retVal;
}

/* dynarr.c */
 
int addNumToDynArr(dynArr *darr, uint64_t n)
{
    if(!darr) return 1;

    if(darr->n_emptySlots == 0) {
        darr->arr = (uint64_t*)realloc(darr->arr, (darr->len + DYNARR_REALLOC_AT_ONCE) * sizeof(dynArr));
        if (!darr->arr) {
            fprintf(stderr, "ERROR: realloc() for dynarr->arr failed!");
            destroyDynArr(darr);
            return 2;
        }
        memset(&darr->arr[darr->n_allocatedSlots], 0, DYNARR_REALLOC_AT_ONCE * sizeof(uint64_t));
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
    darr->arr = (uint64_t*)calloc(sizeof(uint64_t), DYNARR_REALLOC_AT_ONCE);
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
        darr1->arr = (uint64_t*)realloc(darr1->arr, newSize);
        if (darr1->arr == NULL) {
            destroyDynArr(darr1);
            destroyDynArr(darr2);
            return NULL;
        }
    }

    memcpy(&darr1->arr[darr1->len], darr2->arr, darr2->len * sizeof(uint64_t));

    memset(&darr1->arr[darr1->len + darr2->len], 0, (newSize - totalLength) * sizeof(uint64_t));

    darr1->n_allocatedSlots = newSize;
    darr1->n_emptySlots = newSize - totalLength;
    darr1->len = totalLength;

    destroyDynArr(darr2);
    return darr1;
}
