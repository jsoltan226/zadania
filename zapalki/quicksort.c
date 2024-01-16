#include "quicksort.h"
#include <stdlib.h>

static int int64cmp(const void *p, const void *q);

void quicksort(int64_t *A, size_t l)
{
    qsort(A, l, sizeof(int64_t), int64cmp);
}

static int int64cmp(const void *p, const void *q) {
    int64_t x = *(const int64_t *)p;
    int64_t y = *(const int64_t *)q;
    return (x > y) - (x < y);
}
