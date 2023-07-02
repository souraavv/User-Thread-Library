#ifndef _Userthread_h
#define _Userthread_h

#include <ucontext.h>
#include "../include/queue.h"

// four states
#define TERMINATED 0
#define READY 1
#define RUNNING 2
#define BLOCK 3

/*
 * Two queues are maintained
 * ReadyQ : Contains all the tasks which are ready
 * TerminatedQ : Contains all the tasks which are terimanted
 */
struct queue* readyQ, *terminatedQ;


struct attr {
    size_t stackSize; 
    void* stackPtr;   
};

typedef struct attr attr_t;

/*
 * A cheap PCB is called TCB :|
 * Thread Control Block 
 * What do we need from this ? the context of thread
 * Like the whole context, which internally contains register what it was using
 * which state it is now ? 
 * Stack attributes 
 * Return value if we are expecting that, since we don't know type
 * use void* , and later we can typecast accordingly.
 */
struct TCB {
    ucontext_t *context;
    int state, id;
    attr_t* attribute;
    void* retval; 
};
typedef struct TCB tcb_t;

/*
 * Initialize a new thread
 * Block and unblock the thread by passing the thread TCB
 * yield the thread - Forcing thread to leave resource by sending it a single
 * on receiving the signal it will yiled by setting the it's state to termination
 * Thread exits (cleaning)
 * Initial attributes which you want to setup
 * Destroy the attributes setup with the thread, this include stack 
 * 
 * You can create a new thread by passing all what it's structure need
 * you can also init with some default value if want else library will do it for you
 * You can wait for join on a given thread
 * You can also cancel a thread (mean terminate a thread immediately)
 */
void myThread_init();
void myThread_block(struct TCB* thread);
void myThread_unblock(struct TCB* thread);
void myThread_yield(void);
void myThread_exit(void);
void myThread_attr_init(attr_t* attr);
void myThread_attr_destroy(attr_t* attr);

int myThread_create (struct TCB* thread, attr_t* attr ,void*(*func)(void*),void* arg);
int myThread_join(struct TCB* thread, void** status);
int myThread_cancel(struct TCB* thread);

tcb_t* myThread_self(void);

#endif