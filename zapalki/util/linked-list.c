#include "linked-list.h"
#include <stdlib.h>

lls_LinkedList *lls_createLinkedList(void *head_content)
{
    lls_LinkedList *lls = malloc(sizeof(lls_LinkedList));
    if (lls == NULL) return NULL;

    lls_Node *firstNode = lls_createNode(head_content);
    if (firstNode == NULL) {
        free(lls);
        return NULL;
    }

    lls->head = firstNode;
    lls->tail = firstNode;
    return lls;
}

lls_Node *lls_appendNode(lls_Node *referenceNode, void *content)
{
    lls_Node *newNode = calloc(1, sizeof(lls_Node));   
    if (newNode == NULL) return NULL;

    if (referenceNode != NULL) {
		newNode->next = referenceNode->next;
        newNode->previous = referenceNode;
		if (referenceNode->next != NULL) referenceNode->next->previous = newNode;
        referenceNode->next = newNode;
    }

    newNode->content = content;
    return newNode;
}

lls_Node *lls_prependNode(lls_Node *referenceNode, void *content)
{
    lls_Node *newNode = calloc(1, sizeof(lls_Node));   
    if (newNode == NULL) return NULL;

    if (referenceNode != NULL) {
		newNode->previous = referenceNode->previous;
		newNode->next = referenceNode;
		if (referenceNode->previous != NULL) referenceNode->previous->next = newNode;
        referenceNode->previous = newNode;
    }
    
    newNode->content = content;
    return newNode;
}

void lls_destroyNode(lls_Node *node)
{
    if (node->previous != NULL) node->previous->next = node->next;
    if (node->next != NULL) node->next->previous = node->previous;
    free(node);
}

void lls_destroyLinkedList(lls_LinkedList *list)
{
	if (list == NULL) return;

	lls_destroyNodesRecursively(list->head);
	free(list);
}

void lls_destroyNodesRecursively(lls_Node *head)
{
    lls_Node *currentNode = head;
    while (currentNode != NULL) {
		lls_Node *nextNode = currentNode->next;
        free(currentNode);
        currentNode = nextNode;
    };
}
