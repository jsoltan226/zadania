#ifndef INTERVAL_TREE_H
#define INTERVAL_TREE_H

#include <stdint.h>

typedef int64_t itr_Value_t;
typedef uint32_t itr_Int;

typedef struct itr_Node itr_Node;
struct itr_Node {
    itr_Int low, high;
    itr_Node *left_child, *right_child;
    itr_Value_t *value;
};

itr_Value_t itr_Query(itr_Node *root, itr_Value_t value);
int itr_CreateNode(itr_Node *root, itr_Int low, itr_Int high, itr_Value_t value);

#endif /* INTERVAL_TREE_H */
