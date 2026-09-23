#include "tsl.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
TCB threads[TSL_MAXTHREADS];
int scheduling_algorithm; 
int current_thread_id;
int thread_amount;
int threads_index;
TCB que_head;
int ready_threads[TSL_MAXTHREADS];
int lastreadythreadindex;


int tsl_init(int salg);
int tsl_create_thread (void (*tsf)(void *), void *targ);
int tsl_yield (int tid);
int tsl_exit();
void stub (void (*tsf) (void*), void *targ);
int tsl_join(int tid);
int tsl_cancel(int tid);
int tsl_gettid();
int dequeue_ready();
int is_ready(int tid);
int removereadyque(int tid);

int tsl_init(int salg) {
    if (salg != 1 && salg != 2) {
        printf("Only 2 scheduling algorithms supported: 1 for FCFS, 2 for RANDOM\n");
        return TSL_ERROR;
    }

    // Reset thread data
    threads_index = 1;          // main thread will be index 0
    thread_amount = 1;
    scheduling_algorithm = salg;
    lastreadythreadindex=-1;
    for (int i = 0; i < TSL_MAXTHREADS; i++)
    {
        threads[i].state=STATE_IDEAL;
        ready_threads[i]=-1;
        threads[i].tid=-1;
        threads[i].freed=0;
    }
    current_thread_id=TID_MAIN;
    // Initialize main thread TCB (tid 0)
    TCB *main_thread = &threads[0];
    main_thread->tid = TID_MAIN;
    main_thread->state = STATE_RUNNING;
    getcontext(&main_thread->context);
    main_thread->stack = NULL; // main thread uses process stack
    

    return TID_MAIN; 
}


int tsl_create_thread(void (*tsf)(void *), void *targ) {
    if (thread_amount >= TSL_MAXTHREADS) {
        printf("Max number of threads exceeded\n");
        return TSL_ERROR;
    }

    TCB *thread = &threads[threads_index];
    getcontext(&thread->context);

    thread->stack = malloc(TSL_STACKSIZE);
    if (!thread->stack) {
        printf("Failed to allocate stack\n");
        return TSL_ERROR;
    }

    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = TSL_STACKSIZE;
    thread->context.uc_link = NULL;
    thread->tid = threads_index + 1;
    thread->state = STATE_READY;

    // Manual makecontext replacement
    void **stack_top = (void **)((uint8_t *)thread->stack + TSL_STACKSIZE);
    stack_top -= 1; *stack_top = (void *)targ;
    stack_top -= 1; *stack_top = (void *)tsf;
    stack_top -= 1; *stack_top = NULL;  // return address padding

    memset(&thread->context, 0, sizeof(ucontext_t));
    getcontext(&thread->context);
    thread->context.uc_stack.ss_sp = thread->stack;
    thread->context.uc_stack.ss_size = TSL_STACKSIZE;
    thread->context.uc_link = NULL;
#ifdef __x86_64__
    thread->context.uc_mcontext.gregs[REG_RIP] = (greg_t)stub;
    thread->context.uc_mcontext.gregs[REG_RSP] = (greg_t)stack_top;
    thread->context.uc_mcontext.gregs[REG_RDI] = (greg_t)tsf;
    thread->context.uc_mcontext.gregs[REG_RSI] = (greg_t)targ;
#else
#error "Manual context setup is only supported for x86_64"
#endif

    lastreadythreadindex++;
    ready_threads[lastreadythreadindex] = threads_index + 1;

    thread_amount++;
    threads_index++;

    return threads_index;
}

void stub (void (*tsf) (void*), void *targ)
{
    tsf(targ);
    tsl_exit(); 
} 



int tsl_yield(int tid) {
    
    int caller_tid = current_thread_id;
    TCB *caller = &threads[caller_tid-1];
    int next_tid = -1;
    TCB *next;
    
    if(getcontext(&caller->context)==0){
        if (tid > 0) {
            if (is_ready(tid)) {
                next_tid = tid;
            }
            else {
                printf("Thread %d does not exists as ready",tid);
                return TSL_ERROR; 
            }
        }
        else if (tid == TSL_ANY) {
            next_tid = dequeue_ready();
            if (next_tid == -1) {
                
                return tid;
            }
        }
        else {
            return TSL_ERROR;

        }
        int flag = 0;
        if (getcontext(&caller->context) == 0 &&flag ==0) {
            flag = 1;
            if (next_tid != caller_tid) {
                removereadyque(next_tid);
                next = &threads[next_tid-1];
                next->state = STATE_RUNNING;
                
            }
            current_thread_id = next_tid;
            ready_threads[++lastreadythreadindex]=caller_tid;
            caller->state=STATE_READY;
            setcontext(&next->context);
            
            exit(1);
        }
    }   
    caller->state = STATE_RUNNING;
    removereadyque(caller->tid);
    return tid;
}


int tsl_exit(){
    TCB* current_thread=&threads[current_thread_id-1];
    current_thread->state=STATE_TERMINATED;

        int next_tid =-1;
        next_tid=dequeue_ready();
        if(next_tid==-1){
            exit(0);
        }
        TCB *next_thread= &threads[next_tid-1];
        next_thread->state=STATE_RUNNING;
        current_thread_id=next_tid;
        setcontext(&next_thread->context);

        exit(1);
    
    

}

int tsl_join(int tid){

    if (current_thread_id==tid)
    {
        printf("It is not allowed to call tsl_join on it self. It causes deadlock");
        return TSL_ERROR;
    }
    
    if (tid<1 || tid>TSL_MAXTHREADS || threads[tid-1].tid != tid){
        printf("Thread with %d is not exists",tid);
        return TSL_ERROR;
    }

    
    while (threads[tid-1].state!=STATE_TERMINATED){
        tsl_yield(TSL_ANY);
    }

    /*
    threads[tid-1].state=STATE_IDEAL;
    free(threads[tid-1].context.uc_stack.ss_sp);
    free(threads[tid-1].stack);
    threads[tid-1].tid=-1;
    thread_amount--;
    */
    return tid;
}

int tsl_cancel(int tid){
    TCB* current_thread=&threads[tid-1];
    if (current_thread->tid!=tid)
    {
        printf("No thread found with id %d",tid);
        return TSL_ERROR;
    }
    
    if(current_thread->state==STATE_READY){
        removereadyque(current_thread->tid);
        current_thread->state=STATE_TERMINATED; 
    }
    else{
        printf("Thread with id %d is already terminated",tid);
        return TSL_ERROR;
    }
    
    return TSL_SUCCESS;
}

int tsl_gettid(){
    return current_thread_id;
}

int is_ready(int tid){
    for (int i = 0; i < lastreadythreadindex+1; i++)
    {
        if (ready_threads[i]==tid)
        {
            return 1;
        }
    }
    return 0;
}
int dequeue_ready(){
    if(scheduling_algorithm==ALG_FCFS){
        if(lastreadythreadindex==-1){
            
            return -1;
        }
        int a = ready_threads[0];
        for (int i = 0; i < TSL_MAXTHREADS-2; i++)
        {
            ready_threads[i]=ready_threads[i+1];
        }
        ready_threads[TSL_MAXTHREADS-1]=-1;
        lastreadythreadindex--;
        return a;
    }
    else{
        if(lastreadythreadindex==-1){
            return -1;
        }
        else if(lastreadythreadindex==0){
            int a= ready_threads[0];
            lastreadythreadindex--;
            ready_threads[0]=-1;
            return a;
        }
        else{
            srand(time(NULL));
            int index = rand() % (lastreadythreadindex+1);
            int a = ready_threads[index];
            for (int i = index; i < TSL_MAXTHREADS-2; i++)
            {
                ready_threads[i]=ready_threads[i+1];
            }
            ready_threads[TSL_MAXTHREADS-1]=-1;
            lastreadythreadindex--;
            return a;
        }
    }
}
int removereadyque(int tid){
    for(int i=0;i< lastreadythreadindex+1;i++){
        if(ready_threads[i]==tid){
            for (int j = i; j < TSL_MAXTHREADS-1; j++)
            {
                ready_threads[j]=ready_threads[j+1];
            }
            ready_threads[TSL_MAXTHREADS-1]=-1;
            return 1;
        }
    }
    return 0;
}
