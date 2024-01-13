#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarr.h"

/* Cały kod i komentarze bezpośrednio go dotyczące są po angielsku, 
 * ponieważ jestem przyzwyczajony do pisania kodu po angielsku, i pisanie go po polsku jest dla mnie trochę dziwne
 * (A na dodatek w nazwach zmiennych nie mogę używać polskich znaków, co jeszcze bardziej utrudniałoby sprawę)
*/

/* "Długość żadnej z liczb nie przekroczy 10 cyfr." */
#define MAX_DIGITS 10
/* Maksymalna wartość sumy (Pomijając znaki '+', '-' i '='):
 *  (<max długość zapisu> / <max długość liczb>) * <max wartość jednej liczby>
 *
 * "Długość zapisu nie przekroczy 10^6", a zatem:
 *      Maksymalna wartość sumy = (10^6 / 10) * (10^10 - 1) = 10^5 * (10^10 - 1) = 999999999900000
 *
 * 2^63 = 9223372036854775808, a 999999999900000 < 9223372036854775808, więc
 * aby zapisać jedną stronę równania wystarczy nam typ danych liczba całkowita 64-bitowa, 
 * czyli `int64_t` z <stdint.h>.
 */

/* The arguments needed for the `parse_input` function just kept growing, 
 * so I grouped them all into one struct for better code readability */
struct input_info {
    int argc; char **argv;
    dynArr *nums_left; int64_t *sum_left;
    dynArr *nums_right; int64_t *sum_right;
};
int parse_input(struct input_info *info);

dynArr *nums_left, *nums_right;
int64_t sum_left = 0, sum_right = 0;

int main(int argc, char **argv)
{
    /* INPUT PARSING SECTION */
    struct input_info inputInfo = { 
        .argc = argc, .argv = argv,
        .nums_left = nums_left, .sum_left = &sum_left, 
        .nums_right = nums_right, .sum_right = &sum_right 
    };
    if (parse_input(&inputInfo)) {
        fprintf(stderr, "\033[31;1mFATAL ERROR\033[0m: Failed to parse input. Stop.\n");
        return EXIT_FAILURE;
    }

    /* DATA PROCESSING SECTION */
    printf("The difference in the equation is %li.\n", labs(sum_left - sum_right));

    /* CLEANUP SECTION */
    destroyDynArr(nums_left);
    destroyDynArr(nums_right);

    return EXIT_SUCCESS;
}

int parse_input(struct input_info *info)
{
    enum EXIT_CODES {
        EXIT_OK = 0,
        ERR_CREATE_NUMS_LEFT = 1,
        ERR_CREATE_NUMS_RIGHT = 2,
        ERR_INVALID_SUM_PTRS = 3,
        ERR_INVALID_ARGC = 4,
    };

    info->nums_left = createDynArr();
    if (!info->nums_left) {
        fprintf(stderr, "ERROR: failed to create the dynamic array for nums_left.\n");
        return ERR_CREATE_NUMS_LEFT;
    }

    info->nums_right = createDynArr();
    if (!info->nums_right) {
        fprintf(stderr, "ERROR: failed to create the dynamic array for nums_right.\n");
        destroyDynArr(info->nums_left);
        return ERR_CREATE_NUMS_RIGHT;
    }

    if (!info->sum_left || !info->sum_right) {
        fprintf(stderr, "ERROR: The given sum_* int64_t pointers are invalid.\n");
        destroyDynArr(info->nums_left);
        destroyDynArr(info->nums_right);
        return ERR_INVALID_SUM_PTRS;
    }

    if (info->argc >= 2) {
        char buf[MAX_DIGITS + 2] = { 0 };
        int i = 0;
        char *c = info->argv[1];
        dynArr *current_side_darray = info->nums_left;
        int64_t *current_sum_ptr = info->sum_left;
        /* Parse the input string character by character.
         *
         * If the current one is a digit, append it to the buffer.
         * If it's not, convert the current buffer contents to a number, 
         * append it to the appropriate dynarray and reset the buffer.
         *
         * Also, if the character is '=', 
         * it means that we are now on the right sight of the equation,
         * so switch the variables to which processed data is appended accordingly.
         * */
        do {
            if (*c >= '0' && *c <= '9') {
                buf[i] = *c;
                i++;
            } else {
                i = 0;
				if (buf[0] || (!*c && buf[0])) {
                    int64_t num = strtol(buf, NULL, 10);
                    *current_sum_ptr += num;
					addNumToDynArr(current_side_darray, num);
					memset(buf, 0, MAX_DIGITS + 2); /* The buffer must be reset before reuse */
				}

                if (*c == '=') {
                    current_side_darray = info->nums_right;
                    current_sum_ptr = info->sum_right;
                }
                else if (*c == '-') {
                    buf[i] = *c;
                    i++;
                }
            }
        } while(*c++);
    } else {
        fprintf(stderr, "The argument count is too small (minimal is 2, given is %i).\n", info->argc);
        destroyDynArr(info->nums_left);
        destroyDynArr(info->nums_right);
        return ERR_INVALID_ARGC;
    }

    return EXIT_OK;
}
