#ifndef TSL_H
#define TSL_H
#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
// do not change this header file (tsl.h)
// it is the interface of the tsl library to the applications

#include <ucontext.h>

#define STATE_READY 1
#define STATE_RUNNING 2
#define STATE_TERMINATED 3
#define STATE_IDEAL -1

#define TSL_MAXTHREADS 128 // maximum number of threads (including the main thread) that an application can have.
#define TSL_STACKSIZE  32768 // bytes, i.e., 32 KB. This is the stack size for a new thread.

#define ALG_FCFS 1
#define ALG_RANDOM 2
#define ALG_MYALGORITHM 3

#define TID_MAIN 1 // tid of the main tread. this id is reserved for main thread.

#define TSL_ANY 0  // yield to a thread selected with a scheduling alg.

#define TSL_ERROR  -1  // there is an error in the function execution.
#define TSL_SUCCESS 0  // function execution success

typedef struct TCB {
    int tid;
    unsigned int state;
    ucontext_t context;
    char *stack;
    int freed;
} TCB;



int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();
void stub (void (*tsf) (void*), void *targ);

#endif