#include "files.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

char pathOrigen[MAX_PATH_SIZE];
char pathDestino[MAX_PATH_SIZE];

// Given a path + folder name it will create a folder in there
bool createFolder(char *path){
    mkdir(path, 0700);
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
    FILE *sourceFile, *destinationFile;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Open source file for reading
    sourceFile = fopen(source, "rb");
    if (sourceFile == NULL) {
        perror("Error opening source file");
        return 1;
    }

    // Open destination file for writing
    destinationFile = fopen(destination, "wb");
    if (destinationFile == NULL) {
        perror("Error opening destination file");
        fclose(sourceFile);
        return 1;
    }

    // Copy contents from source to destination
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, sourceFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destinationFile);
    }

    // Close files
    fclose(sourceFile);
    fclose(destinationFile);
    return true;
}