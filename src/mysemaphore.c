#include <stdio.h>
#include <stdlib.h>

#include "../include/userthread.h"
#include "../include/queue.h"
#include "../include/mysemaphore.h"
#include "../include/utility.h"

struct mySemaphore* createSemaphore(int cnt) {
    blockPreempt();
    struct mySemaphore* sm = (struct mySemaphore*) malloc (sizeof(struct mySemaphore));
    sm->count = cnt;
    sm->waitQ = createQueue();
    allowPreempt();
    return sm;
}
 // 0 represent success , -1 represent failure;
int down (struct mySemaphore* sm) {
    blockPreempt();
    if (sm == NULL) {
        allowPreempt();
        return -1;
    }
    if(sm->count > 0) {
        --sm->count;
    }
    else {
        struct TCB* curThread = (struct TCB*) front(readyQ);
        enqueue(sm->waitQ, curThread);
        allowPreempt();
        myThread_block(curThread); // remainging to implement
    }
    allowPreempt();
    return 0;
}

int up (struct mySemaphore* sm) {
    blockPreempt();
    if (sm == NULL) {
        allowPreempt();
        return -1;
    }
    if (sizeQueue(sm->waitQ) == 0) {
        sm->count++;
    }
    else {  
        struct TCB* curThread = (struct TCB*)front(sm->waitQ);
        dequeue(sm->waitQ);
        allowPreempt();
        myThread_unblock(curThread);
    }
    allowPreempt();
    return 0;
}

int destroySemaphore(struct mySemaphore* sm) {
    blockPreempt();
    if (sm == NULL) {
        allowPreempt();
        return -1;
    }
    destroyQueue(sm->waitQ);
    free(sm);
    allowPreempt();
    return 0;
}