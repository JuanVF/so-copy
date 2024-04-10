#ifndef FILES_H
#define FILES_H

#include <unistd.h>
#include <stdbool.h>

#define MAX_PATH_SIZE 100
#define BUFFER_SIZE 4096

extern char pathOrigen[MAX_PATH_SIZE];
extern char pathDestino[MAX_PATH_SIZE];

bool createFolder(char *path);
bool copyFile(char *source, char *destination);
bool doesPathExists(char *path);

#endif
