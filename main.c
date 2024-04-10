#include "tree.h"
#include "files.h"
#include "process.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void setNodesByFolderName(struct TreeNode *source);
struct TreeNode* initializeTree(char *sourcePath);
int _initializeTree(struct TreeNode *node, int depth);
void initFolderCopy(struct TreeNode *node, int depth, char *destiny);
void initArchiveCopy(struct LinkedList *archiveList);
void mapTreeToArchiveList(struct LinkedList *archiveList, struct TreeNode *node, int depth, char *destiny);

// setNodesByFolderName will set the nodes given a path
void setNodesByFolderName(struct TreeNode *source) {
    DIR* dir = opendir(source->path);

    if (!doesPathExists(source->path)) {
        return;
    }

    struct dirent* entry;
    struct stat entry_info;

    while ((entry = readdir(dir)) != NULL) {
        bool isDot = strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0;
        
        if (isDot) {
            continue;
        }

        char buffer [2048];

        snprintf(buffer, sizeof(buffer), "%s/%s", source->path, entry->d_name);

        if (stat(buffer, &entry_info) == 0) {
            enum NodeType type;
            enum FileSizeType fileSizeType;

            float size = 0;

            if (S_ISDIR(entry_info.st_mode)) {
                type = FOLDER;
            } else if (S_ISREG(entry_info.st_mode)) {
                type = ARCHIVE;
                fileSizeType = BYTES;

                if (entry_info.st_size >= (1024*1024*1024)) {
                    fileSizeType = GIGABYTES;
                    size = entry_info.st_size / (1024.0*1024.0*1024.0);
                } else if (entry_info.st_size >= (1024*1024)) {
                    fileSizeType = MEGABYTES;
                    size = ((float) entry_info.st_size) / (1024.0*1024.0);
                } else if (entry_info.st_size >= 1024) {
                    fileSizeType = KILOBYTES;
                    size = ((float) entry_info.st_size) / 1024.0;
                }
            } else {
                continue;
            }
            
            struct TreeNode *newNode = createTree(source->path, entry->d_name, type, size, fileSizeType, source);

            appendLinkedListItem(source->children, newNode);
        }
    }

    closedir(dir);
}

// _initializeTree is an auxiliar function for initializeTree
int _initializeTree(struct TreeNode* node, int depth) {
    if (node == NULL || node->type == ARCHIVE) {
        return 0;
    }

    setNodesByFolderName(node);

    for (int i = 0; i < node->children->length; i++) {
        struct TreeNode* value = getLinkedListItem(node->children, i);

        if (value == NULL) {
            break;
        }

        if (strcmp(value->name, "") == 0) {
            continue;
        }

        _initializeTree(value, depth + 1);
    }

    return node->children->length;
}

// initializeTree maps the folder structure to a tree
struct TreeNode* initializeTree(char *sourcePath) {
    struct TreeNode* node = createTree(sourcePath, "", FOLDER, 0, BYTES, NULL);
    
    node->path = strdup(sourcePath);

    _initializeTree(node, 1);

    return node;
}

// initCopy recursively copy each folder from a directory to another
void initFolderCopy(struct TreeNode *node, int depth, char *destiny) {
    if (node == NULL || node->children == NULL || node->type == ARCHIVE) {
        return;
    }

    // resources are limited so we sent items in batches
    int resources = getAmountFreeResources();
    int batches = (node->children->length + resources) / resources;

    for (int b = 0; b < batches; b++) {
        int start = b * resources;
        int end = start + resources;
        int usedResourcesCounter = 0;

        if (end > node->children->length) end = node->children->length; // Make sure we don't go out of bounds

        // We will create each directory items in batches
        for (int i = start; i < end; i++) {
            TreeNode * currentNode = getLinkedListItem(node->children, i);

            if (currentNode == NULL || currentNode->type == ARCHIVE) {
                continue;
            }

            char buffer [PATH_SIZE];
            ProcessItem * process = pickFreeProcess();

            sprintf(buffer, "%s/%s", destiny, currentNode->name);

            onSendNodeMessage(process, buffer, CREATE_FOLDER);

            usedResourcesCounter++;
        }

        // We will wait for pool resources to be free
        for (int i = 0; i < usedResourcesCounter; i++) {
            struct message msg;
            msgrcv(messageQueueId, &msg, sizeof(msg), FATHER_ID, 0);

            setProcessFree(atoi(msg.text));
        }
    }

    for (int i = 0; i < node->children->length; i++) {
        TreeNode * currentNode = getLinkedListItem(node->children, i);

        if (currentNode->type == ARCHIVE) continue;

        char newDestiny[PATH_SIZE];

        sprintf(newDestiny, "%s/%s", destiny, currentNode->name);

        initFolderCopy(currentNode, depth + 1, newDestiny);
    }
}

// mapTreeToArchiveList maps a tree to an archive list
void mapTreeToArchiveList(struct LinkedList *archiveList, struct TreeNode *node, int depth, char *destiny) {
    if (node == NULL || node->children == NULL) {
        return;
    }

    for (int i = 0; i < node->children->length; i++) {
        TreeNode * currentNode = getLinkedListItem(node->children, i);

        if (currentNode == NULL) continue;

        char newDestiny[PATH_SIZE];

        sprintf(newDestiny, "%s/%s", destiny, currentNode->name);

        if (currentNode->type == FOLDER) {
            mapTreeToArchiveList(archiveList, currentNode, depth + 1, newDestiny);
        } else {
            struct TreeNode * mapped = createTree("", "", ARCHIVE, currentNode->size, currentNode->fileSizeType, NULL);

            mapped->path = strdup(newDestiny);
            
            appendLinkedListItem(archiveList, mapped);
        }
    }
}

void initArchiveCopy(struct LinkedList *archiveList) {
    int resources = getAmountFreeResources();
    int batches = (archiveList->length + resources) / resources;

    printf("Largo de la lista %d\n", archiveList->length);

    for (int b = 0; b < batches; b++) {
        int start = b * resources;
        int end = start + resources;
        int usedResourcesCounter = 0;

        if (end > archiveList->length) end = archiveList->length; // Make sure we don't go out of bounds

        // We will create each directory items in batches
        for (int i = start; i < end; i++) {
            TreeNode * currentNode = getLinkedListItem(archiveList, i);

            if (currentNode == NULL || currentNode->type == FOLDER) {
                continue;
            }

            char buffer [PATH_SIZE];
            ProcessItem * process = pickFreeProcess();

            strcpy(buffer, currentNode->path);

            onSendNodeMessage(process, buffer, CREATE_ARCHIVE);

            usedResourcesCounter++;
        }

        // We will wait for pool resources to be free
        for (int i = 0; i < usedResourcesCounter; i++) {
            struct message msg;
            msgrcv(messageQueueId, &msg, sizeof(msg), FATHER_ID, 0);

            setProcessFree(atoi(msg.text));
        }
    }
}

int main (int argc, char *argv[]){
    // Manually inputs the folders path
    if (argc <= 2) {
        printf("Por favor indique la ubicación de la carpeta que se desea copiar: \n");
        fgets (pathOrigen, sizeof(pathOrigen), stdin);
        pathOrigen[strcspn(pathOrigen, "\n")] = 0;

        printf("Por favor indique la ubicación de la carpeta donde desea pegar los archivos: \n");
        fgets (pathDestino, sizeof(pathDestino), stdin);
        pathDestino[strcspn(pathDestino, "\n")] = 0;

    // Receive the folder paths through parameters
    } else {
        strncpy(pathOrigen, argv[1], MAX_PATH_SIZE - 1);
        strncpy(pathDestino, argv[2], MAX_PATH_SIZE - 1);
    }

    if (!doesPathExists(pathOrigen)) {
        char message [100];
        printf(message, "La ubicación origen \"%s\" ingresada no existe \n", pathOrigen);
        return 1;
    }

    if (!doesPathExists(pathDestino)) {
        printf("Creando folder destino \"%s\"\n", pathDestino);
        createFolder(pathDestino);
    }

    initMessageQueue();
    initProcessPool();

    if (isFather) {
        struct TreeNode *sourceNode = initializeTree(pathOrigen);

        if (sourceNode == NULL) {
            printf("Algo ha salido mal...");
            freeProcessPool();

            return 1;
        }

        initFolderCopy(sourceNode, 0, "");

        struct LinkedList * archiveList = (struct LinkedList*) malloc(sizeof(struct LinkedList));

        archiveList->length = 0;
        archiveList->firstNode = NULL;
        archiveList->lastNode = NULL;

        mapTreeToArchiveList(archiveList, sourceNode, 0, "");

        initArchiveCopy(archiveList);

        // let all process die
        for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
            if (processPool[i] != NULL) {
                char buffito [200];
                strcpy(buffito, "KILLING");
                onSendNodeMessage(processPool[i], buffito, KILLING);
            }
        }

        for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
            if (processPool[i] != NULL) {
                wait(NULL);
            }
        }

        freeProcessPool();

        if (isFather) {
            msgctl(messageQueueId, IPC_RMID, NULL);
        }

        printf("Saliendo...\n");
    }

    return 0;
}