#ifndef U_HASHTABLE_H
#define U_HASHTABLE_H

#include "linked-list.h"

#define TABLE_SIZE  10

#define HT_RESIZING_FACTOR 5
#define HT_MAX_KEY_LENGTH 256

typedef struct {
    char key[HT_MAX_KEY_LENGTH];
    void *value;
} ht_Record;

typedef struct {
    unsigned int length;
    unsigned int n_elements;
	lls_LinkedList **bucketLists;
} ht_HashTable;

ht_HashTable *ht_createTable(unsigned int length);
int ht_insert(ht_HashTable *ht, const char *key, void *entry);
void *ht_lookupValue(ht_HashTable *ht, const char *key);
void ht_deleteRecord(ht_HashTable *ht, const char *key);
void ht_printTable(ht_HashTable *ht);
void ht_destroyTable(ht_HashTable *ht);

#endif /* U_HASHTABLE_H */
