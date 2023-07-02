#include <stdio.h>
#include <stdlib.h>

#include "../include/userthread.h"
#include "../include/mysemaphore.h"

static semaphore_t* lock;

int criticalSection = 0;

void* func(void* arg) {
    int tid = *(int*)arg;
    for (int i = 1; i <= 25; ++i) {
        down(lock);
        criticalSection++; // do something in critical section.
        printf ("Thread id: %d, crititcal_section = %d\n", tid, criticalSection);
        
        myThread_yield();
        up(lock);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int n = 5;
    lock = createSemaphore(1);
    myThread_init();
    tcb_t* threads = (tcb_t*)malloc(n * sizeof(tcb_t));
    int* threadId = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i) {
        threadId[i] = i + 1;
        myThread_create(&threads[i], NULL, func, (void*)&threadId[i]);
    }
    for (int i = 0; i < n; ++i) {
        myThread_join(&threads[i], NULL);
    }
    free(threadId);
    return 0;
}