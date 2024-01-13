#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS 10

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

dynArr *nums_left, *nums_right;

int main(int argc, char **argv)
{
    enum EXIT_CODES {
        EXIT_OK = 0,
        ERR_CREATE_NUMS_LEFT = 1,
        ERR_CREATE_NUMS_RIGHT = 2,
    };

    nums_left = createDynArr();
    if (!nums_left) {
        fprintf(stderr, "ERROR: failed to create the dynamic array for nums_left.\n");
        return ERR_CREATE_NUMS_LEFT;
    }

    nums_right = createDynArr();
    if (!nums_right) {
        fprintf(stderr, "ERROR: failed to create the dynamic array for nums_right.\n");
        destroyDynArr(nums_left);
        return ERR_CREATE_NUMS_RIGHT;
    }


    if (argc == 2) {
        char buf[MAX_DIGITS + 2] = { 0 };
        int i = 0;
        char *c = argv[1];
        dynArr *current_side_darray = nums_left;
        do {
            if (*c >= '0' && *c <= '9') {
                buf[i] = *c;
                i++;
            } else {
                i = 0;
				if (buf[0] || (!*c && buf[0])) {
					addNumToDynArr(current_side_darray, strtol(buf, NULL, 10));
					memset(buf, 0, MAX_DIGITS + 2);
				}

                if (*c == '=') current_side_darray = nums_right;
                else if (*c == '-') {
                    buf[i] = *c;
                    i++;
                }
            }
        } while(*c++);
    }

    printf("left side of the equation: ");
    for (int i = 0; i < nums_left->len; i++) {
        printf("%li; ", nums_left->arr[i]);
    }
    printf("\nright side of the equation: ");
    for (int i = 0; i < nums_right->len; i++) {
        printf("%li; ", nums_right->arr[i]);
    }
    putchar('\n');

    destroyDynArr(nums_left);
    destroyDynArr(nums_right);

    return EXIT_OK;
}

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
        memset(&darr->arr[darr->n_emptySlots - 1], 0, DYNARR_REALLOC_AT_ONCE * sizeof(int64_t));
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
