#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

#include <stdint.h>
#include <stddef.h>

/* Perform a binary search for target value `T` in sorted array `A` with the length of `l`.
 * Return the index of `T`, or the one of the element immidiately to the left of where `T` should be. */
size_t binary_search(int64_t *A, size_t l, int64_t T);

#endif /* BINARY_SEARCH_H */
