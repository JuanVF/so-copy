#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <stdlib.h>

#define POOL_PROCESS_LENGTH 5

typedef struct ProcessItem {
    int id;
    pid_t pid;
    bool isFree;
} ProcessItem;

void onSendNodeMessage(ProcessItem * process);
void onMessageReceived(ProcessItem * process);
bool initMessageQueue();
bool initProcessPool();
void freeProcessPool();

int messageQueueId;
int isFather;

struct ProcessItem *childInfo;

struct ProcessItem *processPool[POOL_PROCESS_LENGTH];

#endif