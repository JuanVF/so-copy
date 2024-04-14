#include "process.h"
#include "tree.h"
#include "files.h"

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

int messageQueueId = -1;  // Assuming existing definition
int isFather = 1;         // Assuming existing definition
struct ProcessItem *childInfo = NULL; // Define if not already defined
struct ProcessItem *processPool[POOL_PROCESS_LENGTH] = {NULL}; // Define if not already defined
char pathCSV [512];
char csvFileName[100];

// onSendNodeMessage will send messages to an specific process
void onSendNodeMessage(ProcessItem * process, char message [PATH_SIZE], enum ProcessState mode) {
    process->isFree = false;

    struct message msg;

    msg.mode = mode;
    msg.type = process->id + 1;
    strcpy(msg.text, message);

    int status = msgsnd(messageQueueId, &msg, sizeof(msg), 0);
}

// onMessageReceived will be a listener for messages
void onMessageReceived(ProcessItem * process) {
    while (1) {
        struct message msg;
        int id = process->id;

        int status = msgrcv(messageQueueId, &msg, sizeof(msg), id + 1, 0);

        if (status < 0) {
            usleep(1000 * 100);
            continue;
        }

        if (msg.mode == KILLING) {
            break;
        }

        if (msg.mode == CREATE_FOLDER) {
            char buffer [1028];

            if (msg.text[0] == '/') {
                sprintf(buffer, "%s%s", pathDestino, msg.text);
            } else {
                sprintf(buffer, "%s/%s", pathDestino, msg.text);
            }
            printf("creando folder %s\n", buffer);

            createFolder(buffer);
        }

        // Creating CSV FILE for testing
        strncpy(csvFileName,"Bitacora_Prueba_Pool7.csv", MAX_PATH_SIZE - 1);// csvFileName = "Bitacora_Prueba_Pool1.csv";
        sprintf(pathCSV, "%s/%s",pathDestino, csvFileName);
        bool exists = doesPathExists(pathCSV);
        if (!exists){
            FILE *fpt;
            fpt = fopen(pathCSV, "w+");
            fprintf(fpt, "File, Process, Time\n");
            fclose(fpt);
            printf("************Se creo el archivo CSV************\n");
        }

        if (msg.mode == CREATE_ARCHIVE) {
            // time measure
            time_t start, end;
            double dif;
            time(&start);
            // time measure
            char bufferOrigen [1028];
            char bufferDestino [1028];

            if (msg.text[0] == '/') {
                sprintf(bufferOrigen, "%s%s", pathOrigen, msg.text);
                sprintf(bufferDestino, "%s%s", pathDestino, msg.text);
            } else {
                sprintf(bufferOrigen, "%s/%s", pathOrigen, msg.text);
                sprintf(bufferDestino, "%s/%s", pathDestino, msg.text);
            }
            printf("copiando archivo: %s hacia %s\n", bufferOrigen, bufferDestino);

            copyFile(bufferOrigen, bufferDestino);
            //time measure
            time(&end);
            dif = difftime (end, start);
            //time measure
            // EDITING CSV FILE//
            FILE *fpt;
            fpt = fopen(pathCSV, "a");
            fprintf(fpt, "%s, %d, %f\n", msg.text, process->pid, dif);
            fclose(fpt);
            // EDITING CSV FILE //
        }

        struct message fatherMsg;
        fatherMsg.type = FATHER_ID;

        sprintf(fatherMsg.text, "%d", id);
        status = msgsnd(messageQueueId, &fatherMsg, sizeof(fatherMsg), 0);

        if (status < 0) {
            printf("whoopsie error sending to father...\n");
        }
    }
}

void setProcessFree(int id) {
    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        if (processPool[i] != NULL && processPool[i]->id == id) {
            processPool[i]->isFree = true;
        }
    }
}

// getAmountFreeResources returns the amount of free resources
int getAmountFreeResources() {
    int counter = 0;
    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        if (processPool[i] != NULL && processPool[i]->isFree) {
            counter++;
        }
    }

    return counter;
}

// pickFreeProcess will pick the first free process
struct ProcessItem * pickFreeProcess() {
    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        if (processPool[i] != NULL && processPool[i]->isFree) {
            return processPool[i];
        }
    }

    return NULL;
}

// initMessageQueue will initialize the message queue
bool initMessageQueue() {
    srand(time(NULL));

    int randomId = rand() % 255;
    int status;
    key_t messageQueueKey = ftok("files.c", randomId);

    if (messageQueueKey < 0) messageQueueKey *= -1;

    messageQueueId = msgget(messageQueueKey, 0666 | IPC_CREAT);

    return true;
}

// freeProcessPool will free the pool memory used. It assumes there is no memory garbage.
void freeProcessPool() {
    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        if (processPool[i] != NULL) {
            free(processPool[i]);
        }
    }
}

// initProcessPool will allocate memory to store the process pool data
// it assumes the message queue is already been created
bool initProcessPool() {
    // make sure all the values in the process pool are NULL pointers by default. No memory garbage
    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        processPool[i] = NULL;
    }

    for (int i = 0; i < POOL_PROCESS_LENGTH; i++) {
        processPool[i] = (struct ProcessItem*) malloc(sizeof(struct ProcessItem));

        if (processPool[i] == NULL) {
            return false;
        }

        pid_t pid = fork();

        processPool[i]->id = i;
        processPool[i]->pid = pid;
        processPool[i]->isFree = true;

        if (pid != 0) {
        } else {
            processPool[i]->pid = getpid();
            isFather = 0;
            childInfo = processPool[i];
            // Fork will be stuck receiving messages and then break the for
            onMessageReceived(childInfo);
            break;
        }
    }

    return true;
}