#include "files.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

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

// Copies a file from source to destination using a buffer.
bool copyFile(char *source, char *destination) {
    return true;
}