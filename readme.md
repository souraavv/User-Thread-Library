## Introduction
Threads are used to introduce parallelism in the code. In Case if we have a multi-core system we can effectively schedule our thread over the different core and can run our code parallel. They are also referred as the lightweight process. They can follow a complete independent sequence of instructions within a running process. Multiple threads within the same process can share the same code and data section. 

Each thread also have a data structure associated with it that is called TCB(Thread Control Block) which stores the state of the thread for example It stores the value of the registers that are used by the thread in case of a switch. Each thread has its local data which is generally not shared in stack space, a set of registers and a PC(Program Counter).


## User Thread Library

In this project, I builded a toy user thread library. Threads at user levels are generally managed without any kernel supports. Kernel are not aware of these threads. Generally these are introduced by the programmer in their
implementation. In our implementation of myUserThread we have implemented functions
below. First we have defined a structure which which will store information about the
thread.

## Structure of user thread library

```c
typedef struct TCB {
    ucontext_t* context;
    int state, id;
    struct attr* attibutes;
    void* return_value;
} tcb_t;

typedef struct attr {
    size_t stack_size;
    void* stack_ptr;
}attr_t;

```

## Features

1. `myThread init()` : This is very first function which we call. This basically initialize
the ready queue and the terminated queue. Along with that this function is used
to push the thread of our main program into the ready queue. It also setup the
setittimer function which periodically sends a SIGALRM signal at the period of
50ms.
2. `myThread create(thread,attr,start routine,arg)` : This function is used to
create the user thread. It takes four(4) parameters. First one is a pointer to the
thread to which memory was allocated by the user using malloc. Second is the attr
which contains detail about stack size and stack pointer if user pass them else it
is passed NULL. Third parameter is start routine which is the function that this
thread will be executing. And last is the arg to this functions.
3. `myThread exit()` : It basically set the state of TCB to the TERMINATED state
and also enqueue it to the terminated queue and call the myThread yield.
4. `myThread cancel(thread)` : It removed the thread TCB from the ready queue
and push it to the terminated queue.
5. `myThread attr init(attr)` : It is used to initialize the thread attributes. If user
has passed then it is set from that. Else if user pass NULL then we initialize it with
the default values.
6. `myThread attr destroy(attr)` : It basically destroy the attributes which we have
set in the TCB thread.
7. `myThread self(void)` : This simply return the a pointer to the current TCB itself
which is present in the front of the ready queue.
8. `myThread yield` : Yield is used to raise the signal SIGALRM immediately and
basically that then call to the scheduler which in turn either swap or set the new
context.
9. `myThread join(thread)` : This basically wait for the thread to complete.

## Synchronization

Additionally it supports sync mechanism between threads. Semaphore is simply a variable that is shared between threads. It is used to solve the
critical section problem and also provide synchronization in the code.

Methods used in the User Semaphore Library
1. `createSemaphore(val)` : It initialize the semaphore with the val provided by the
user. It also allocate the queue which is waiting queue for this semaphore.
2. `down(sm)` : It basically down the semaphore count. If it is not possible then it
push the thread which is calling semaphore down in it’s waiting queue.
3. `up(sm)` : It basically increase the count by one. If there are still some TCB in the
waiting queue then it only push the front of the waiting queue into the ready queue
without incrementing.
4. `destroySemaphore` : It basically free the waiting queue allocated on the heap.


## Use of Signals
Signals are used to generate interrupts. We can use signal with the help of signal.h header
file. The main function that are used in our code are.
1. `sigemptyset(sigset t* st)` : This basically initializes the signal set given by st to
empty means no signal is included.
2. `sigaddset()` : This adds the signal passed to the signal set.
3. `sigprocmask(how, *newset, *oldset)` is used to change the signal
mask, the set of currently blocked signals.


## Test

### Multi threaded Matrix Multiplication with N threads
- Here we have implemented matrix multiplication with the help of the user thread library
that we have implemented. Along with that because our result matrix is a shared resource
between these N threads we also need semaphore.

### Bounded Buffer with N containers and M producer and consumers

- We have buffer which is of size N. And we have M producer and N consumers. We are
also using two counting semaphore full and empty. And a binary semaphore which is used to access the common resource between producer and consumer i.e the buffer.


## System Specifications

1. CPU(s) : 2
2. Socket(s) : 2
3. Core(s) per socket : 1
4. Thread(s) per core : 1

## Software Requirements
1. GNU Compiler Collections (GCC).
2. Git Version Control System.

## References

References
1. [Linux manual on make context - 1](https://pubs.opengroup.org/onlinepubs/7908799/xsh/getcontext.html)
2. [Linux manual on make context - 2](https://man7.org/linux/man-pages/man3/makecontext.3.html)
3. [Unix system call](https://www.tutorialspoint.com/unix_system_calls/sigprocmask.htm)
4. [Linux manual for pthread](https://man7.org/linux/man-pages/man7/pthreads.7.html)