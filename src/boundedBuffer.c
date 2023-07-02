#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/userthread.h"
#include "../include/mysemaphore.h"

int n, m;

semaphore_t *empty;
semaphore_t *full;
semaphore_t *mutex;

int* buffer;
int idx = 0, idx2 = 0;
int globalConsumed = 0, globalProduced = 0;

// producer.
void* myProducer(void* arg){
    int prodId = *(int*)arg;
    while (globalProduced < n * 10) {
        down(empty);
        down(mutex);
        
        int curItem = globalProduced;
        buffer[idx] = curItem;
        printf ("Thread: %d, Produced: %d, idx: %d\n", prodId, globalProduced, idx);
        idx = (idx + 1) % n;
        globalProduced++;
        up(mutex);
        up(full);
    }
    return NULL;   
}

//consumer:
void* myConsumer (void* arg) {
    int consId = *(int*)arg;
    while (globalConsumed < n * 10) {
        down(full);
        down(mutex);
        int curItem = buffer[idx2];
        printf ("Thread: %d, Consumed: %d, idx: %d\n", consId, curItem, idx2);
        idx2 = (idx2 + 1) % n;
        globalConsumed++;
        up(mutex);
        up(empty);
        

    }
    return NULL;
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        printf("very few argument: n, m\n");
        exit(0);
    }
    n = atoi(argv[1]);
    m = atoi(argv[2]);
    mutex = createSemaphore(1);
    full = createSemaphore(0);
    empty = createSemaphore(n);

    tcb_t* producer = (tcb_t*) malloc(m * sizeof(tcb_t));
    tcb_t* consumer = (tcb_t*) malloc(m * sizeof(tcb_t));

    buffer = (int*) malloc(n * sizeof(int));
    myThread_init();
    int* threadId = (int*) malloc(m * sizeof(int));
    
    for (int i = 0; i < m; ++i) {
        threadId[i] = i;
        myThread_create(&producer[i], NULL, myProducer, (void*)&threadId[i]);
        myThread_create(&consumer[i], NULL, myConsumer, (void*)&threadId[i]);
    }
    for (int i = 0; i < m; ++i) {
        myThread_join(&producer[i], NULL);
    }
    for (int i = 0; i < m; ++i) {
        myThread_join(&consumer[i], NULL); 
    }

    destroySemaphore(full);
    destroySemaphore(empty);
    destroySemaphore(mutex);
    
    free(threadId);
    return 0;
}
