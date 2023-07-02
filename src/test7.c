#include <stdio.h>
#include <stdlib.h>

#include "../include/userthread.h"
#include "../include/mysemaphore.h"

static semaphore_t* lock;

int criticalSection = 0;

void* func1(void* arg) {
    int tid = *(int*)arg;
    for (int i = 1; i <= 50; ++i) {
        down(lock);
        criticalSection++; // do something in critical section.
        printf ("Thread id: %d, crititcal_section = %d\n", tid, criticalSection);
        myThread_yield();
        up(lock);
    }
    return NULL;
}

void* func2 (void* arg) {
    int tid = *(int*)arg;
    for (int i = 1; i <= 50; ++i) {
        up(lock);
        criticalSection++; // do something in critical section.
        printf ("Thread id: %d, crititcal_section = %d\n", tid, criticalSection);
        myThread_yield();
        down(lock);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int n = 5;
    lock = createSemaphore(0);
    myThread_init();
    tcb_t* threads = (tcb_t*)malloc(n * sizeof(tcb_t));
    int* threadId = (int*)malloc(n * sizeof(int));

    void* (*func_ptr[2]) (void*) = {func1, func2};
    for (int i = 0; i + 1 < n; ++i) {
        threadId[i] = i + 1;
        myThread_create(&threads[i], NULL, func_ptr[0], (void*)&threadId[i]);
    }
    threadId[n - 1] = n;
    myThread_create(&threads[n - 1], NULL, func_ptr[1], (void*)&threadId[n - 1]);
    for (int i = 0; i < n; ++i) {
        myThread_join(&threads[i], NULL);
    }
    free(threadId);
    return 0;
}