#include "binary-search.h"
#include <stdint.h>

size_t binary_search(int64_t *A, size_t l, int64_t T)
{
    size_t left = 0, right = l - 1;
    size_t middle;

    while (left != right) {
        middle = (right + left) / 2 + ((right + left) % 2 != 0);
        if (A[middle] > T) {
            right = middle - 1;
        } else {
            left = middle;
        };
    }

    return left;
}
