#ifndef FILES_H
#define FILES_H

#include <unistd.h>
#include <stdbool.h>

#define MAX_PATH_SIZE 100

char pathOrigen[MAX_PATH_SIZE];
char pathDestino[MAX_PATH_SIZE];

bool createFolder(char *path);
bool createFile(char *path, unsigned char *data, size_t dataSize);
bool doesPathExists(char *path);

#endif