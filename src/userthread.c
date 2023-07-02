#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/queue.h"
#include "../include/userthread.h"
#include "../include/utility.h"
 
static int Count = 0;
const int STACKSIZE = 640000;

/*
 * This is the function to which each thread points, to which we are passing
 * the user function which user want to service by the thread, with the given args
 * we are setting the retval in case user need that, once are done with this 
 * we are exiting the thread.
 * yThread_exit() : handles the change of state of the thread to TERMINATE, enqueue it to the
 *                  terminated queue as well as yield the resource immediately so that another 
 *                  thread can run immediately.
*/
static void runProgram(void*(*func)(void*), void* arg) {
    tcb_t* cur = (tcb_t*)front(readyQ);
    cur->retval = (*func)(arg);
    // blockPreempt and allowPreempt allow us to do this task atomically.
    blockPreempt();
    myThread_exit();
    allowPreempt();
}

/*
 * This is the initialization of my thread library
 * We first setup the both queue
 *      - Ready Queue
 *      - Terminated Queue
 * This also push the thread of our main program into the ready queue
 * So it first create a TCB which it will use to store the context of the current thread
 * We used getContext which will take the context of the thread and store it to the pointer
 * which we have passed to it. (We initialize that pointer which can hold ucontext_t* type of object)
 * 
 * We put the thread state to running
 * We are also maintaining a global count of how many threads we have in system using count.
 * 
 * To perform few operation atomic we have used blokcPreempt() and allowPreempt()
 * Before inserting the threads into queue we block preemption so that we fully perform this task
 */
void myThread_init() {
    readyQ = (struct queue*) createQueue();
    terminatedQ = (struct queue*)createQueue();
    
    struct TCB* firstThread = (struct TCB*)malloc(sizeof(struct TCB));
    firstThread->context = (ucontext_t*) malloc(sizeof(ucontext_t));
    getcontext(firstThread->context);
    firstThread->id = Count++;
    firstThread->state = RUNNING;

    blockPreempt();
    enqueue(readyQ, (void*)firstThread);
    allowPreempt();

    setUpTimer();
}
/*
    This function is used to create the user thread. 
    It takes four(4) parameters. 
     - First one is a pointer to the thread to which memory was allocated by the user using malloc. 
     - Second is the attr which contains detail about stack size and stack pointer if user pass them else it
       is passed NULL. 
     - Third parameter is start routine which is the function that this
       thread will be executing.
     - And last is the arg to this functions.

*/
int myThread_create (tcb_t* thread, attr_t* attr ,void*(*func)(void*),void* arg) {
    if (!thread) {
        return -1;
    }
    // create a new context, assuming user is passing the tcb object, our 
    // library will not create that, so overhead of taking out tcb finally (freeing)
    // will be done by user it self.

    thread->context = (ucontext_t*) malloc (sizeof(ucontext_t));
    if (!thread->context) {
        return -1;
    }
    // we allocated a unique id to the user thread.
    thread->id = Count++;
    // setup the thread state to be running
    thread->state = READY;
    // If user has not provided attributes tot the thread, then we are using default,
    // else we are utilizing the one provided by the user.
    if(attr == NULL) {
        attr_t* temp = (attr_t*) malloc(sizeof(attr_t));
        myThread_attr_init(temp);
        thread->attribute = temp;
    }
    else {
        thread->attribute = attr;
    }
    getcontext(thread->context);
    thread->context->uc_stack.ss_sp = thread->attribute->stackPtr;
    thread->context->uc_stack.ss_size = thread->attribute->stackSize;
    thread->context->uc_stack.ss_flags = 0;
    thread->context->uc_link = NULL;

    // registering the stack, size, flags and the fuction it need to call 
    // we are keeping a general function runProgram, which internally will
    // call to the function which user has mentioned in func with the arg
    // func is a function pointer.
    makecontext(thread->context, (void(*)(void))runProgram, 2, func, arg);
    
    blockPreempt();
    // insert this new thread into the readyQ
    enqueue(readyQ, thread);
    allowPreempt();
    return 0;
}

void myThread_yield(void) {
    raise(SIGALRM);
}

// set the state of current thread to termination, and also push
// its context to the terminated queue and call the thread yield, immediately
// so that the scheduler can run, without wasting the remaining cycles

void myThread_exit(void) {
    blockPreempt();
    struct TCB* curThread = (struct TCB*)front(readyQ);
    curThread->state = TERMINATED;
    enqueue(terminatedQ, curThread);
    allowPreempt();
    myThread_yield();
}

// stop the execution of current thread, remove it from the ready queue
// and push to the termination queue

int myThread_cancel(struct TCB* thread) {
    int tid = thread->id, isPresent = 0, firstTime = 1;
    struct TCB* ourFront = (struct TCB*) front(readyQ);
    struct TCB* curFront = ourFront;
    while (firstTime == 1 || ourFront != curFront) {
        firstTime = 0;
        if (curFront->id == tid) {
            enqueue(terminatedQ, curFront);
            dequeue(readyQ);
            isPresent = 1;
            if (curFront == ourFront)
                break;// not possible case : but still handle it
        }
        else {
            enqueue(readyQ, (struct TCB*)front(readyQ));
            dequeue(readyQ);
        }
        curFront = (struct TCB*) front(readyQ);
    }
    return isPresent ? 0 : -1;
}

// block the thread by changing the state to the block
void myThread_block(struct TCB* thread) {
    thread->state = BLOCK;
    myThread_yield();
} 

// ublock a thread, by setting state to unblock and insert it to the ready queue
void myThread_unblock(struct TCB* thread) {
    thread->state = READY;
    blockPreempt();
    enqueue(readyQ, thread);
    allowPreempt();
}

// Init the stack with the attributes.
void myThread_attr_init(attr_t* attr) {
    attr->stackSize = STACKSIZE;
    attr->stackPtr = (void*)malloc(STACKSIZE);
}

// free the resources.
void myThread_attr_destroy(attr_t* attr) {
    free(attr->stackPtr);
    free(attr);
}

// join the thread, a waiting call.
int myThread_join(struct TCB* thread, void** status) {
    blockPreempt();
    int presentInReadyQ = 0, presentInTerminatedQ = 0;
    presentInReadyQ = queueFind(readyQ, thread->id);
    allowPreempt();
    while(1) {
        blockPreempt();
        // see if it is present in the terminated queue
        struct TCB* store = queueFindAndReturn(terminatedQ, thread->id);
        if (store) {
            presentInReadyQ = 1;
            presentInTerminatedQ = 1;
            // collect the status from the thread, so that we can return to the user.
            if (status) {
                *status = store->retval;
            }
            // now you can safely destory the attributes 
            myThread_attr_destroy(store->attribute);
            // also remove it from the terminated queue.
            dequeue(terminatedQ); 
        }
        allowPreempt();
        // ? 
        if (presentInReadyQ == 0) {
            return -1;
        }
        // we break once we find that it is in terminated queue
        if (presentInTerminatedQ) {
            break;
        }
        // and then we call thread yield, so that this can raise signal.
        myThread_yield();
    }
    return 0;
}

// simply return a the first tcb in the queue.
tcb_t* myThread_self(void) {
    if (readyQ == NULL) {
        return NULL;
    }
    return (tcb_t*)front(readyQ);
}