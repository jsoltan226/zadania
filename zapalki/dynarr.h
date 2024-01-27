#ifndef DYNARR_H
#define DYNARR_H

#include <stdint.h>

/* Ummm.... WTF is an std::vector????????? */

#define DYNARR_REALLOC_AT_ONCE  100
typedef struct {
    int64_t *arr;
    uint32_t len, n_allocatedSlots;
    uint32_t n_emptySlots;
} dynArr;
int addNumToDynArr(dynArr *darr, int64_t n);
dynArr *createDynArr();
void destroyDynArr(dynArr *darr);
dynArr *mergeDynArrs(dynArr *darr1, dynArr *darr2);

#endif /* DYNARR_H */
