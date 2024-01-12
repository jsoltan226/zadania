#ifndef U_LINKED_LIST_H
#define U_LINKED_LIST_H

typedef struct lls_Node lls_Node;
struct lls_Node {
    lls_Node *next;
    lls_Node *previous;
    void *content;
};

typedef struct {
    lls_Node *head;
    lls_Node *tail;
} lls_LinkedList;

lls_LinkedList *lls_createLinkedList(void *head_content);

lls_Node *lls_appendNode(lls_Node *referenceNode, void *content);
lls_Node *lls_prependNode(lls_Node *referenceNode, void *content);
#define lls_createNode(content) lls_appendNode(NULL, content)

void lls_destroyLinkedList(lls_LinkedList *list);
void lls_destroyNodesRecursively(lls_Node *head);
void lls_destroyNode(lls_Node *node);

#endif /* U_LINKED_LIST_H */
