#include "hashtable.h"
#include "linked-list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static unsigned int hash(const char *key, unsigned int max);
static lls_Node *lookupBucketListNode(ht_HashTable *ht, const char *key);
ht_HashTable *ht_createTable(unsigned int length)
{
    ht_HashTable *ht = malloc(sizeof(ht_HashTable));
    if (ht == NULL) return NULL;

    ht->length = length;
    ht->n_elements = 0;

    ht->bucketLists = calloc(length, sizeof(lls_LinkedList*));
    if (ht->bucketLists == NULL) {
        free(ht);
        return NULL;
    }

    return ht;
}

int ht_insert(ht_HashTable *ht, const char *key, void *entry)
{
    if (ht == NULL) return 1;

    int index = hash(key, ht->length);

	ht_Record *newRecord = malloc(sizeof(ht_Record));
	if (newRecord == NULL) return 1;

	newRecord->value = entry;
	strncpy(newRecord->key, key, HT_MAX_KEY_LENGTH);
	newRecord->key[HT_MAX_KEY_LENGTH - 1] = '\0';

	if (ht->bucketLists[index] == NULL) {
		ht->bucketLists[index] = lls_createLinkedList(newRecord);
		if (ht->bucketLists[index] == NULL) {
			free(newRecord);
			return 1;
		}
	} else {
		ht->bucketLists[index]->head = lls_appendNode(ht->bucketLists[index]->head, newRecord);
	}

	ht->n_elements++;

    return 0;
}

void *ht_lookupValue(ht_HashTable *ht, const char *key)
{
    lls_Node *node = lookupBucketListNode(ht, key);
    if (node == NULL) return NULL;
    else if(node->content == NULL) return NULL;
    else return ((ht_Record*)(node->content))->value;
}

void ht_deleteRecord(ht_HashTable *ht, const char *key)
{
    lls_Node *node = lookupBucketListNode(ht, key);
	free(node->content);
    lls_destroyNode(node);
}

void ht_destroyTable(ht_HashTable *ht)
{
    for (int i = 0; i < ht->length; i++) {
        if (ht->bucketLists[i] != NULL) lls_destroyLinkedList(ht->bucketLists[i]);
    }
    ht->bucketLists = NULL;

    free(ht);
}

static unsigned int hash(const char *key, unsigned int max)
{
    return key[0] % max;
}

static lls_Node *lookupBucketListNode(ht_HashTable *ht, const char *key)
{
    int index = hash(key, ht->length);

	if (ht->bucketLists[index] == NULL) return NULL;

    lls_Node *currentNode = (ht->bucketLists[index])->tail;
    while (currentNode != NULL) {
		if (currentNode->content != NULL) {
			if (!strncmp(((ht_Record*)currentNode->content)->key, key, HT_MAX_KEY_LENGTH)) return currentNode;
		}
        currentNode = currentNode->next;
	}

    return NULL;
}
