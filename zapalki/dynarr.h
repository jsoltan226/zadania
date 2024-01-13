#ifndef DYNARR_H
#define DYNARR_H

#include <stdint.h>

/* Ummm.... WTF is an std::vector????????? */
#define DYNARR_REALLOC_AT_ONCE  5
typedef struct {
    int64_t *arr;
    int len, n_allocatedSlots;
    unsigned int n_emptySlots;
} dynArr;
int addNumToDynArr(dynArr *darr, int64_t n);
dynArr *createDynArr();
void destroyDynArr(dynArr *darr);


#endif /* DYNARR_H */
