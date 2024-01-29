#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarr.h"
#include "quicksort.h"

/* Cały kod i komentarze (oprócz poniższego objaśnienia działania programu) są po angielsku, 
 * ponieważ jestem przyzwyczajony do pisania kodu po angielsku, i pisanie go po polsku jest dla mnie trochę dziwne
 * (A na dodatek w nazwach zmiennych nie mogę używać polskich znaków, co jeszcze bardziej utrudniałoby sprawę)
*/

/* "Długość żadnej z liczb nie przekroczy 10 cyfr." */
#define MAX_DIGITS 10

/* Maksymalna wartość sumy (Włączając znaki '+', '-' i '='):
 *  (<max długość zapisu> / <max długość liczb + 1 (dla '+' i '-')>) * <max wartość jednej liczby> + 1 (pierwsza liczba nie musi mieć znaku '+') - 2 (dla '=' i jednej cyfry na drugiej stronie równania)
 *
 * "Długość zapisu nie przekroczy 10^6", a zatem:
 *      Maksymalna wartość sumy = (10^6 / 11) * (10^10 - 2) = 909090908909091
 *
 * 2^63 = 9223372036854775808, a 909090908909091 < 9223372036854775808, więc
 * aby zapisać jedną stronę równania wystarczy nam typ danych liczba całkowita 64-bitowa, 
 * czyli `int64_t` z <stdint.h>.
 */
#define MAX_SUM 909090908909091

/* Ponieważ operujemy na liczbach nie większych niż 10^10-1 (999999999) i nie mniejszych niż -9999999999,
 * to możemy założyć, że jeżeli różnica między sumami danych stron równań przekracza maksymalną wartość,
 * o którą możemy zmienić wynik przestawiając 1 zapałkę, równania nie da się naprawić.
 * Wartość tą wyliczyłem na >9000000000<, zmieniając pierwsze '9' w liczbie na '0' w taki sposób:
 *    =====               =====                       =====               =====
 *   |     |             |     |                     |     |             |     |
 *   |     |             |     |                     |     |             |     |
 * -  =====  999999999 =  =====  999999999  ====>  -  =====  999999999 = |     | 999999999
 *         |                   |                           |             |     |
 *         |                   |                           |             |     |
 *    =====               =====                       =====               =====
 * Taka postać rzeczy umożliwia nam w wielu przypadkach wczesne zakończenie działania programu,
 * bez wykonywania dużych ilośći sprawdzań i obliczeń.
 */
#define MAX_EQUATION_DIFFERENCE 9000000000
/* Z podobnego założenia możemy wyjść na poziomie porównywania konkretncyh liczb;
 * Jeżeli różnica między nimi jest za duża,
 * to możemy przejść do następnej pary bez dużych ilości zgadywania.
 * Ta wartość jest taka sama co ta dotycząca całego równania.
 */
#define MAX_NUM_DIFFERENCE MAX_EQUATION_DIFFERENCE


dynArr *nums_left = NULL, *nums_right = NULL, *nums_merged = NULL;
int64_t sum_left = 0, sum_right = 0;

static dynArr *sort_and_remove_duplicates(dynArr *arr);
static void cleanup(const char *msg, int code);

/* Some shortucts for reducing the amount of error-checking code */

/* Initially there was a 'cleanup' label which would be called with `goto`,
 * but the c++ compiler pretty much forbids the useage of it,
 * so I had to replace the label with a normal function. */
#define exit_clean(message, code) do { \
        cleanup(message, code); \
/* Instead of the call above, it looked something like this:
 *      fprintf(code != 0 ? stderr : stdout, "%s", message);
 *      goto cleanup;
*/ \
        return code; \
    } while(0);
/* And yes, the only reason I didn't rewrite the cleanup/exit stuff is because I'm lazy. Deal with it. */

#define exit_no_solution() exit_clean("no", EXIT_SUCCESS)

int main(int argc, char **argv)
{
    if (argc != 2) return EXIT_FAILURE;

    /* Don't waste (potentially 10^6 bytes of) memory just to have a separate output message buffer */
    char *out_msg = argv[1];
    uint32_t out_msg_len = 0; /* To not waste resources, this will be calculated in parse_input() */

    /* INPUT PARSING SECTION */
    enum EXIT_CODES {
        EXIT_OK = 0,
        ERR_CREATE_NUMS_LEFT = 1,
        ERR_CREATE_NUMS_RIGHT = 2,
        ERR_INVALID_SUM_PTRS = 3,
        ERR_REMOVE_DUPLICATES = 4,
        ERR_MERGE_EQUATION_SIDES = 5,
    };

    nums_left = createDynArr();
    if (!nums_left)
        exit_clean("ERROR: failed to create the dynamic array for nums_left.\n", ERR_CREATE_NUMS_LEFT);

    nums_right = createDynArr();
    if (!nums_right)
        exit_clean("ERROR: failed to create the dynamic array for nums_right.\n", ERR_CREATE_NUMS_RIGHT);

    char buf[MAX_DIGITS + 2] = { 0 };
    int i = 0;
    char *str = argv[1];
    dynArr *current_side_darray = nums_left;
    int64_t *current_sum_ptr = &sum_left;
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
        char c = *str;
        out_msg_len++;

        if (c >= '0' && c <= '9') {
            buf[i] = c;
            i++;
        } else {
            i = 0;
			if ((buf[0] >= '0' && buf[0] <= '9') || buf[0] == '-') {
                int64_t num = strtol(buf, NULL, 10);
                *current_sum_ptr += num;
				addNumToDynArr(current_side_darray, num);
				memset(buf, 0, MAX_DIGITS + 2); /* The buffer must be reset before reuse */
			}
            if (c == '=') {
                current_side_darray = nums_right;
                current_sum_ptr = &sum_right;
            } else if (c == '-') {
                buf[0] = '-';
                i++;
            }
        }
    } while(*str++);
    out_msg_len--;

#ifdef DEBUG
    printf("before: \nnums_left:\n\t[ ");
    for (uint32_t i = 0; i < nums_left->len; i++) {
        printf("%li, ", nums_left->arr[i]);
    }
    printf("\b\b ];\nnums_right:\n\t[ ");
    for (uint32_t i = 0; i < nums_right->len; i++) {
        printf("%li, ", nums_right->arr[i]);
    }
    printf("\b\b ];\n\nafter: ");
#endif

    /* INITIAL PROCESSING SECTION */
    const int64_t difference = labs(sum_left) - labs(sum_right);

    /* Exit early if the equation is already valid */
    if (difference == 0) exit_clean(out_msg, EXIT_SUCCESS);

    /* Exit early if the difference in the equation is greater than the maximum change we can make */
    if (labs(difference) > MAX_EQUATION_DIFFERENCE) exit_no_solution();

    /* Move all numbers to one size of the equation */
    dynArr *nums_longer = NULL, *nums_shorter = NULL;
    if (nums_left->len > nums_right->len) {
        nums_longer = nums_left;
        nums_shorter = nums_right;
    } else {
        nums_longer = nums_right;
        nums_shorter = nums_left;
    }
    for (uint32_t i = 0; i < nums_shorter->len; i++) {
        nums_shorter->arr[i] = -(nums_shorter->arr[i]);
    }

    nums_merged = mergeDynArrs(nums_longer, nums_shorter);
    if (!nums_merged)
        exit_clean("FATAL ERROR: Failed to merge nums_left with nums_right.\n", ERR_MERGE_EQUATION_SIDES);
    nums_left = nums_right = nums_shorter = nums_longer = NULL;


    /* The array must be sorted for binary_search() to work properly */
    nums_merged = sort_and_remove_duplicates(nums_merged);
    if (!nums_merged)
        exit_clean("FATAL ERROR: Failed to remove duplicates from nums_merged.\n", ERR_REMOVE_DUPLICATES);

    /* MAIN GUESSING SECTION */
    memset(out_msg, 0, strlen(out_msg));

#ifdef DEBUG
    printf("\nnums_merged:\n\t[ ");
    for (uint32_t i = 0; i < nums_merged->len; i++) {
        printf("%li, ", nums_merged->arr[i]);
    }
    printf("\b\b ];\n");
#endif

    for (uint32_t i = 0; i < nums_merged->len; i++) {
        int64_t currentNum = nums_merged->arr[i];
        currentNum++;
    }
    exit_clean(out_msg, EXIT_SUCCESS);
}

static dynArr *sort_and_remove_duplicates(dynArr *darr)
{
    /* (TODO: Optimize this part so as to not loop throught the array multiple times) */

    if (!darr) return NULL;
    if (darr->len < 2) return darr;

    quicksort(darr->arr, darr->len);

    /* Remove duplicates from sorted array */
    dynArr *newDynArray = createDynArr();
    if (!newDynArray) return NULL;

    if (addNumToDynArr(newDynArray, darr->arr[0])) return NULL;
    for (uint32_t i = 1; i < darr->len; i++) {
        if (darr->arr[i] != darr->arr[i - 1]) {
            if (addNumToDynArr(newDynArray, darr->arr[i])) return NULL;
        }
    }
    destroyDynArr(darr);
    
    return newDynArray;
}

static void cleanup(const char *msg, int code)
{
    fprintf(code != 0 ? stderr : stdout, "%s\n", msg);

    /* CLEANUP SECTION */
    if (nums_left) destroyDynArr(nums_left);
    if (nums_right) destroyDynArr(nums_right);
    if (nums_merged) destroyDynArr(nums_merged);
}
