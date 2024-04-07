#include "process.h"

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

int messageQueueId = -1;
int isFather = 1;

// onMessageReceived will 
void onMessageReceived(ProcessItem * process) {
}

// initMessageQueue will initialize the message queue
bool initMessageQueue() {
    int status;
    key_t messageQueueKey = 999;

    messageQueueId = msgget(messageQueueKey, IPC_CREAT | S_IRUSR | S_IWUSR);

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

        if (pid != 0) {
        } else {
            processPool[i]->pid = getpid();
            isFather = 0;
            childInfo = processPool[i];
            // Fork will be stuck receiving messages and then break the for
            onMessageReceived(processPool[i]);
            break;
        }
    }

    return true;
}