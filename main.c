#include "tree.h"
#include "files.h"
#include "process.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

void setNodesByFolderName(struct TreeNode *source);
struct TreeNode* initializeTree(char *sourcePath);
int _initializeTree(struct TreeNode *node, int depth);

// setNodesByFolderName will set the nodes given a path
void setNodesByFolderName(struct TreeNode *source) {
    DIR* dir = opendir(source->path);

    if (!doesPathExists(source->path)) {
        return;
    }

    struct dirent* entry;
    struct stat entry_info;

    char buffer [2048];

    while ((entry = readdir(dir)) != NULL) {
        bool isDot = strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0;
        
        if (isDot) {
            continue;
        }

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

        spanTabs(depth);
        if (value->type == FOLDER) {
            printf("-%s/ \n", value->name);
        } else {
            char buffer[100];

            if (value->fileSizeType == BYTES){
                strcpy(buffer, "Bytes");
            }

            if (value->fileSizeType == KILOBYTES){
                strcpy(buffer, "KB");
            }

            if (value->fileSizeType == MEGABYTES){
                strcpy(buffer, "MG");
            }

            if (value->fileSizeType == GIGABYTES){
                strcpy(buffer, "GB");
            }

            printf("-%s [SIZE=%.2f %s]\n", value->name, value->size, buffer);
        }

        _initializeTree(value, depth + 1);
    }

    return node->children->length;
}

// initializeTree maps the folder structure to a tree
struct TreeNode* initializeTree(char *sourcePath) {
    struct TreeNode* node = createTree(sourcePath, "", FOLDER, 0, BYTES, NULL);
    
    printf("%s\n", node->path);
    _initializeTree(node, 1);

    return node;
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
    } else if (argc >= 3) {
        strncpy(pathOrigen, argv[1], MAX_PATH_SIZE - 1);
        strncpy(pathDestino, argv[2], MAX_PATH_SIZE - 1);
    } else {
        printf("Numero de argumentos no permitido. Debe ser <path_origen> <path_destino> o  <path_origen> <path_destino> --enable-debug\n");
        return 1;
    }

    // we don't want all prints to be showed
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--enable-debug") == 0) {
        }
    }

    if (!doesPathExists(pathOrigen)) {
        char message [100];
        sprintf(message, "La ubicación origen \"%s\" ingresada no existe \n", pathOrigen);
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

        for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
            if (processPool[i] != NULL) {
                wait(NULL);
            }
        }

        printf("Saliendo...\n");
    }

    freeProcessPool();

    return 0;
}