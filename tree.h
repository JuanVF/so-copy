#ifndef TREE_H
#define TREE_H

enum NodeType {
    ARCHIVE = 0,
    FOLDER
};

enum FileSizeType {
    BYTES = 0,
    KILOBYTES,
    MEGABYTES,
    GIGABYTES
};

typedef struct TreeNode TreeNode;
typedef struct LinkedList LinkedList;

typedef struct TreeNode {
    struct TreeNode * father;
    struct TreeNode *next;
    struct TreeNode *previous;
    struct LinkedList * children;
    char *path;
    char *name;
    enum NodeType type;
    float size;
    enum FileSizeType fileSizeType;
} TreeNode;

typedef struct LinkedList {
    struct TreeNode *firstNode;
    struct TreeNode *lastNode;
    int length;
} LinkedList;

struct TreeNode * createTree(char *path, char *name, enum NodeType type, float size, enum FileSizeType fileSizeType, struct TreeNode *father);

struct TreeNode* getLinkedListItem(struct LinkedList *list, int i);
int appendLinkedListItem(struct LinkedList *list, struct TreeNode *item);

void spanTabs(int n);

#endif