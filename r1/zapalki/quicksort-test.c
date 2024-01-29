#ifndef no_tests
#include "binary-search.h"
#include "nums.h"
#include "quicksort.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    size_t nums_length = sizeof(nums) / sizeof(int64_t);
    quicksort(nums, nums_length);

    if (argc >= 1 && argv[1]) {
        int64_t n = strtol(argv[1], NULL, 10);
        printf("index of %li in `nums` is %li.\n", n, binary_search(nums, nums_length, n));
    }
    for (int i = 0; i < 1000000; i++) {
        binary_search(nums, nums_length, i);
    }
    printf("nums[%li] = '%li'\n", 0l, nums[0]);
    printf("nums[%li] = '%li'\n", 1l, nums[1]);
    printf("nums[%li] = '%li'\n", nums_length - 1, nums[nums_length - 1]);
    printf("nums[%li] = '%li'\n", nums_length - 2, nums[nums_length - 2]);
    return EXIT_SUCCESS;
}
#endif
