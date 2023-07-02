/*
 * Below we are using guards,
 * They can help us in multiple inclusions
 * And also in avoiding circular inclusions
 */

#ifndef _Semaphore_h
#define _Semaphore_h

/**
 * @brief
 * What is a Semaphore ? 
 * A semaphore is used in synchronization 
 * It need to have a waiting queue so that it can hold those 
 * who want resource but are not able to get immediately
 * We are using counting semaphore, by setting the value of count = 1
 * we can also use it as the binary semaphore
 * @implements
 * What operation we are allowing in semaphore 
 */

struct mySemaphore {
    struct queue* waitQ;
    int count;
};

/*
 * typedef in C is used to alias the existing names.
 */

typedef struct mySemaphore semaphore_t; 

/**
 * @implements
 * What functions we are exposing ? 
 *  - Create an instance of semaphore
 *  - Down on a semaphore
 *  - Up on a semaphore
 *  - Destroy the semaphore : As C don't destroy the items which are
 *    dynamically allocated, as a programmer it is pain to recollect all those
 *    garbage 
 */
struct mySemaphore* createSemaphore(int cnt);
int down (struct mySemaphore* sm);
int up (struct mySemaphore* sm);
int destroySemaphore(struct mySemaphore* sm);

#endif