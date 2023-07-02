#include <stdio.h>
#include <stdlib.h>

#include "../include/userthread.h"

void* func(void* arg) {
    for (int i = 0; i <= 50; ++i) {
        printf ("Thread is in the iteration := %d\n", i);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    myThread_init();
    tcb_t* thread = (tcb_t*) malloc (sizeof(tcb_t));
    myThread_create(thread, NULL, func, NULL);
    myThread_join(thread, NULL);
    free(thread);
    return 0;
}