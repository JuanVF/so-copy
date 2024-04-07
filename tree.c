#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// spanTabs prints n "\t"
void spanTabs(int n) {
    for (int i = 0; i < n; i++) {
        printf("   ");
    }
}

// Initialize a Tree given a source path
struct TreeNode * createTree(char *path, char *name, enum NodeType type, float size, enum FileSizeType fileSizeType, struct TreeNode *father) {
    struct TreeNode * sourceNode = (struct TreeNode *) malloc(sizeof(struct TreeNode));

    if (sourceNode == NULL) {
        return NULL;
    }

    char buffer [2048];

    sprintf(buffer, "%s/%s", path, name);

    sourceNode->name = strdup(name);
    sourceNode->path = strdup(buffer);
    sourceNode->next = sourceNode;
    sourceNode->previous = sourceNode;
    sourceNode->type = type;
    sourceNode->size = size;
    sourceNode->father = father;
    sourceNode->fileSizeType = fileSizeType;

    if (sourceNode->name == NULL || sourceNode->path == NULL) {
        free(sourceNode);

        return NULL;
    } 

    sourceNode->children = (struct LinkedList*) malloc(sizeof(struct LinkedList));

    if (sourceNode->children == NULL) {
        free(sourceNode);

        return NULL;
    }

    return sourceNode;
}

// it will return a linked list item by its id
struct TreeNode* getLinkedListItem(struct LinkedList *list, int index) {
    if (list == NULL) {
        return NULL;
    }

    struct TreeNode *currentNode = list->firstNode;

    for (int i = 0; i < list->length; i++) {
        if (currentNode == NULL) {
            return NULL;
        }

        if (i == index) {
            return currentNode;
        }

        currentNode = currentNode->next;
    }

    return NULL;
}

// will append an item to the linked list and return its index
int appendLinkedListItem(struct LinkedList *list, struct TreeNode *item) {
    if (list == NULL) {
        return -1;
    }

    if (list->lastNode == NULL) {
        list->lastNode = item;
        list->firstNode = item;
    }

    list->lastNode->next = item;
    list->lastNode = item;
    list->lastNode->next = list->firstNode;
    list->firstNode->previous = item;
    list->length++;

    return list->length;
}