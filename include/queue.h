#ifndef _Queue_h_
#define _Queue_h_

/**
 * @details
 * 
 */
struct container {
    struct container* next;
    void* data;
};

struct queue {
    int sz;
    struct container *start;
    struct container *end;
};

typedef struct queue queue_t;

struct queue* createQueue(void);
void enqueue(struct queue* q, void* data);
void dequeue(struct queue* q);
int isEmpty(struct queue* q);
void* front(struct queue* q);
void printQueue(struct queue* q);
int sizeQueue(struct queue* q);
int pushBack(struct queue* q);
int queueFind(struct queue* q, int tid);
void* queueFindAndReturn(struct queue* q, int tid);
void destroyQueue(struct queue* q);

#endif