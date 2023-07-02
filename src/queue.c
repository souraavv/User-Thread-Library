#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/queue.h"
#include "../include/userthread.h"



struct queue* createQueue() {
    struct queue* q = (struct queue*)malloc(sizeof(struct queue));
    if (q == NULL) 
        return NULL;
    q->start = NULL;
    q->end = NULL;
    q->sz = 0;
    return q;
}

void enqueue(struct queue* q, void* curData) {
    struct container* cnew = (struct container*)malloc(sizeof(struct container));
    cnew->data = curData;
    cnew->next = NULL;
    if (q-> end) {
        q->end->next = cnew;
        q->end = cnew;
    }
    else {
        q->start = q->end = cnew;
    }
    q->sz++;
}

void dequeue(struct queue* q) {
    if (q == NULL || isEmpty(q)) {
        printf ("can't apply deque to alraedy emtpy queue\n");
        return;
    }
    q->start = q->start->next;
    --q->sz;
    if(q->sz == 0) {
        q->end = q->start = NULL;
    }
    return;
}

int pushBack(struct queue* q) {
    if (!q->end) {
        return 0;
    }
    q->end->next = q->start;
    q->end = q->start;
    q->start = q->start->next;
    q->end->next = NULL;
    return 1;
}
int isEmpty(struct queue* q) {
    return q->sz == 0;
}

int sizeQueue(struct queue* q) {
    return q->sz;
}

void* front(struct queue* q) {
    if (q->start == NULL) {
        return NULL;
    }
    return q->start->data;
}

int queueFind(struct queue* q, int tid) {
    int sz = sizeQueue(q);
    struct container* itr = q->start;
    for (int i = 0; i < sz; ++i) {
        struct TCB* temp = (struct TCB*)itr->data;
        if (temp->id == tid) {
            return 1;
        }
        itr = itr->next;
    }
    return 0;
}

void* queueFindAndReturn(struct queue* q, int tid) {
    int sz = sizeQueue(q);
    // jo queue ke front main hain agar bo match na karein
    
    for (int i = 0; i < sz; ++i) {
        struct TCB* temp = (struct TCB*)front(q);
        if (temp->id == tid) {
            return temp;
        }
        else {
            pushBack(q);
        }
    }
    return NULL;
}

void destroyQueue(struct queue* q) {
    while (q->start) {
        struct container* temp = q->start;
        q->start = q->start->next;
        free(temp);
    }
    free(q);
}