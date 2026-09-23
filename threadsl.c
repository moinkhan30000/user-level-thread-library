#include "threadsl.h"
#include <stdio.h>

TCB threads[TSL_MAXTHREADS];
int scheduling_algorithm; 
int current_thread_id;
int thread_amount;
int threads_index;
TCB que_head;

int tsl_init(int salg){
    if(salg != 1 || salg != 2){
        printf("2 thread algorithm is available. 1 for ALG_FCFS 2 for ALG_RANDOM");
        return TSL_ERROR;
    }
    threads_index=0;
    thread_amount=0;
    que_head=NULL;
    scheduling_algorithm = salg;
    return TSL_SUCCESS;
}

int tsl_create_thread (void (*tsf)(void *), void *targ){
    if (thread_amount >= TSL_MAXTHREADS){
        printf("Max number of threads is exceeded");
        return TSL_ERROR;
    }
    TCB thread;
    getcontext(&thread.context);
    thread.stack=(char *)malloc(TSL_STACKSIZE);
    if(!thread.stack){
        printf("Stack could not be arranged");
        return TSL_ERROR;
    }
    
    thread.state=1;
    thread.id=++threads_index;

}

int tsl_yield (int tid){

}

int tsl_exit(){

}

int tsl_join(int tid){

}

int tsl_cancel(int tid){

}

int tsl_gettid(){
    return current_thread_id;
}
