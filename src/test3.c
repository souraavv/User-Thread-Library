#include <stdio.h>
#include <stdlib.h>

#include "../include/userthread.h"

void* func(void* arg) {
    int tid = *(int*)arg;
    for (int i = 1; i <= 25; ++i) {
        printf ("Thread id: %d, i = %d\n", tid, i);
        if (i % 5 == 0) {
            myThread_yield();
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    int n = 5;
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