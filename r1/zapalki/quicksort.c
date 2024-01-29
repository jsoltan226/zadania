#include "quicksort.h"
#include <stdlib.h>

static int int64cmp(const void *a, const void *b);

void quicksort(int64_t *A, size_t l)
{
    qsort(A, l, sizeof(int64_t), int64cmp);
}

static int int64cmp(const void *a, const void *b) {
    int64_t x = *(const int64_t*)a;
    int64_t y = *(const int64_t*)b;
    return (x > y) - (x < y);
}
