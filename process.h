#ifndef PROCESS_H
#define PROCESS_H

#include "tree.h"
#include <stdbool.h>
#include <stdlib.h>

#define POOL_PROCESS_LENGTH 10
#define PATH_SIZE 200
#define FATHER_ID POOL_PROCESS_LENGTH + 1

enum ProcessState {
    CREATE_FOLDER,
    CREATE_ARCHIVE,
    KILLING
};

typedef struct ProcessItem {
    int id;
    pid_t pid;
    bool isFree;
} ProcessItem;

struct message {
    long type;
    enum ProcessState mode;
    char text[PATH_SIZE];
};

void onSendNodeMessage(ProcessItem * process, char message [PATH_SIZE], enum ProcessState mode);
void onMessageReceived(ProcessItem * process);
bool initMessageQueue();
bool initProcessPool();
void freeProcessPool();
void setProcessFree(pid_t id);
struct ProcessItem * pickFreeProcess();
int getAmountFreeResources();

extern int messageQueueId;
extern int isFather;

extern struct ProcessItem *childInfo;

extern struct ProcessItem *processPool[POOL_PROCESS_LENGTH];

#endif
