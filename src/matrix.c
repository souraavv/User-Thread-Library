#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#include "../include/userthread.h"
#include "../include/mysemaphore.h"

int** A, **B, **C, **D;

sem_t binarySem;
semaphore_t* mutex;

int n, nthread;
static int thread_cnt;
double startTime, endTime, runTime;

double getCurrentTime () {
    struct timeval cur;
    gettimeofday (&cur, NULL);
    return (cur.tv_sec * (double)1000000) + cur.tv_usec;
}

void fillMatrix (int **a, int n, int flag) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (flag)
                a[i][j] = rand() % 10;
            else   
                a[i][j] = 0;
        }
    }
}

void printMatrix(int **mat, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf ("%d ", mat[i][j]);
        }
        printf ("\n");
    }
    printf ("\n-----------------------------------\n");
}

void* matrixMult(void* arg) {
    int tid = *(int*)arg;
    int start = tid * ((n + thread_cnt - 1) / thread_cnt);
    int end = start + (n + thread_cnt - 1) / thread_cnt;
    if (end > n) {
        end = n;
    }
    down(mutex);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int tij = 0;
            for (int k = start; k < end; ++k) {
                tij += A[i][k] * B[k][j];
            }
            C[i][j] += tij;
        }
    }
    up(mutex);
    return NULL;
}


void* matrixMult2(void* arg) {
    int tid = *(int*)arg;
    int start = tid * ((n + thread_cnt - 1) / thread_cnt);
    int end = start + (n + thread_cnt - 1) / thread_cnt;
    if (end > n) {
        end = n;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int tij = 0;
            sem_wait (&binarySem);
            for (int k = start; k < end; ++k) {
                tij += A[i][k] * B[k][j];
            }
            
            C[i][j] += tij;
            sem_post (&binarySem);
        }
    }
    
    return NULL;
}

void normalMult() {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                D[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void verify (int **a, int **b, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (a[i][j] != b[i][j]) {
                printf("There is some issue!\n");
                return;
            }
        }
    }
    printf("Both results are same.\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf ("very few arguments\n");
        exit(0);
    }
    n = atoi(argv[1]);
    nthread = atoi(argv[2]);

    FILE* ptr1, *ptr2;
    ptr1 = fopen ("mat.dat", "w");
    ptr2 = fopen ("mat2.dat", "w");
    A = (int **) malloc (n * sizeof (int*));
    B = (int **) malloc (n * sizeof (int*));
    C = (int **) malloc (n * sizeof (int*));
    D = (int **) malloc (n * sizeof (int*));

    for (int i = 0; i < n; ++i) {
        A[i] = (int *) malloc (n * sizeof (int));
        B[i] = (int *) malloc (n * sizeof (int));
        C[i] = (int *) malloc (n * sizeof (int));
        D[i] = (int *) malloc (n * sizeof (int));
    }
    
    fillMatrix(A, n, 1);
    fillMatrix(B, n, 1);
    fillMatrix(C, n, 0);
    fillMatrix(D, n, 0);

    normalMult();
    
    myThread_init();
    mutex = createSemaphore(1);
    for (thread_cnt = 1; thread_cnt <= nthread; ++thread_cnt) {
        printf ("-------------\nNumber of threads %d\n", thread_cnt);
        tcb_t* threads = (tcb_t*) malloc (thread_cnt * sizeof(tcb_t));    
        int* threadId = (int*) malloc (thread_cnt * sizeof(int));
        fillMatrix(C, n, 0);

        startTime = getCurrentTime();
         for (int i = 0; i < thread_cnt; ++i) {
            threadId[i] = i;
            myThread_create(&threads[i], NULL, matrixMult, (void*)&threadId[i]);
        }
        for (int i = 0; i < thread_cnt; ++i) {
            myThread_join(&threads[i], NULL);
        }
        endTime = getCurrentTime();
        runTime = (endTime - startTime) / 1000000;
        printf ("Time in user_thread: %f\n", runTime);
        double avgTime = (runTime) / (double)thread_cnt;
        fprintf(ptr1, "%d\t\t%f\t\t%f\n", thread_cnt, runTime, avgTime);

        // ---- pthread
        sem_init (&binarySem, 0, 1);
        fillMatrix(C, n, 0);
        pthread_t *ptr = (pthread_t*) malloc (thread_cnt * sizeof(pthread_t));
        if (ptr == NULL) {
            return 0;
        }

        startTime = getCurrentTime();
        for (int i = 0; i < thread_cnt; ++i) {
            pthread_create(&ptr[i], NULL, matrixMult2, (void*)&threadId[i]);
        }
        for (int i = 0; i < thread_cnt; ++i) {
            pthread_join (ptr[i], NULL);
        }

        endTime = getCurrentTime();
        runTime = (endTime - startTime) / 1000000;
        printf ("Time in pthread: %f\n", runTime);
        avgTime = (runTime) / (double)thread_cnt;
        fprintf(ptr2, "%d\t\t%f\t\t%f\n", thread_cnt, runTime, avgTime);
        free(ptr);
        free(threadId);
        destroySemaphore(mutex);
        mutex = createSemaphore(1);
    }
    fclose (ptr1);
    fclose (ptr2);
    return 0;
}