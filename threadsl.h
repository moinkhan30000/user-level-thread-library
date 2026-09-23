#ifndef THREADSL_H
#define THREADSL_H

#include <ucontext.h>

#define TSL_ERROR -1
#define TSL_SUCCESS 0
#define TSL_ANY 0
#define TSL_MAXTHREADS 128
#define TSL_STACKSIZE 32768

typedef struct TCB {
    int tid;
    unsigned int state;
    ucontext_t context;
    char *stack;
    TCB *next;
} TCB

int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();

#endif