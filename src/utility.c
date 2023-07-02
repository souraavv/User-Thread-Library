#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/queue.h"
#include "../include/userthread.h"

sigset_t signalMask;

struct sigaction mysig; 
struct itimerval myTime;

const int myInterval = 50000;

void allowPreempt() {
    sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
}

void blockPreempt() {
    sigprocmask(SIG_BLOCK, &signalMask, NULL);
}

static void scheduler () {
    // pick the tcb of the thread which in right now executing or is in the
    // front of the queue.
    struct TCB* oldThread = (struct TCB*) front(readyQ);
    // check what is its state (running, terminated, block, ready) ? 
    int checkTerminate = (oldThread->state == TERMINATED);
    int checkBlock = (oldThread->state == BLOCK);
    if (oldThread->state == RUNNING) { // if it needs cpu still, change it's state to the ready 
        oldThread->state = READY;
    }
    if (checkTerminate || checkBlock)  // if it block or terminated, then remove it from the readyQ
        dequeue(readyQ); // remove it from the queue.
    else 
        pushBack(readyQ); // else if it not then we push it back to the readyQ
        // pushBack is taking the front and moving it to the back, O(1) both operation in a single
        // we can also take the response of the pushBack, ignore here.

    //printf ("\n---- I am scheduler ---- %d\n", sizeQueue(readyQ));
    struct TCB* newThread = (struct TCB*) front(readyQ); // ok now take the tcb of new thread
    bool checkSame = (newThread == oldThread); // if it is the same thread, in case we only have single thread left
   
    newThread->state = RUNNING; // update the thread state to the running
    
    if (checkSame) {
        return; // we don't need to bring it's context as they are already same, optimization
    }
    // if not terminated, then we swap the context, because we need to restore it back, as the old thread
    // is not yet done.
    if (!checkTerminate) {
        // we actually provde two things here, where to store the old context and from where to load the 
        // new context
        swapcontext(oldThread->context, newThread->context); // else we swap the context and the system will bring the new context to the cpu
    }
    else {
        // in case old thread is done, then we can overwrite the context.
        setcontext(newThread->context);
    }
}

void setUpTimer() {
    // create signal mask
    sigemptyset(&signalMask);
    // add SIGALRM
    sigaddset(&signalMask, SIGALRM);

    // when this signal is raised, we call the scheduler
    mysig.sa_handler = &scheduler;
    sigemptyset(&mysig.sa_mask);
    sigaddset(&mysig.sa_mask, SIGALRM); //setting up the signal : sigalrm
    mysig.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGALRM, &mysig, NULL);
    
    myTime.it_value.tv_sec = 0;
    myTime.it_value.tv_usec = myInterval;
    myTime.it_interval.tv_sec = 0;
    myTime.it_interval.tv_usec = myInterval;

    setitimer(ITIMER_REAL, &myTime, NULL);
}

