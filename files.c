#include "files.h"
#include <stdbool.h>
#include <stdio.h>

// Given a path + folder name it will create a folder in there
bool createFolder(char *path){
    return true;
}

// doesPathExists verifies if a given path exists
bool doesPathExists(char *path){
    FILE *fp = fopen(path, "r");
    bool is_exist = false;
    if (fp != NULL){
        is_exist = true;
        fclose(fp); // close the file
    }
    return is_exist;
}

// Creates a file by its path and data
bool createFile(char *path, unsigned char *data, size_t dataSize) {
    return true;
}