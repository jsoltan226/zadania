#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef uint32_t Product_ID;

#define DAY_MIN 1
#define DAY_MAX 1000000000
typedef struct {
    uint32_t from, to;
} DayRange;

typedef struct {
    DayRange *prev, *next;
    DayRange range;
    DayRange **overlapping_ranges; size_t n_overlapping_ranges;
} IntervalTree;

typedef struct {
    Product_ID product_id;
    DayRange range;
    uint32_t diff;
} SpecialOffer;

typedef struct {
    uint32_t price;
} Product;

uint32_t best_day = 0, final_price = 0;
uint32_t n_products = 0, n_special_offers = 0;

int main(void)
{
    scanf("%u %u", &n_products, &n_special_offers);
    
    Product products[n_products];
    SpecialOffer offers[n_special_offers];

    for (Product_ID productID = 0; productID < n_products; productID++) {
        scanf("%u", &products[productID].price);
        scanf("%c", NULL);
    }

    uint32_t max_day_from = DAY_MIN;
    for (int i = 0; i < n_special_offers; i++) {
        SpecialOffer *so = &offers[i];
        scanf("%u %u %u %u\n", &so->product_id, &so->range.from, &so->range.to, &so->diff);
        so->diff = products[so->product_id - 1].price - so->diff;
        max_day_from = so->range.from > max_day_from ? so->range.from : max_day_from;
    }


    return 0;
}
