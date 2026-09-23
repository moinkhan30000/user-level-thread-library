#include <stdio.h>
#include "tsl.h"

void thread_function_1(void *arg) {
    printf("[Thread %d] Starting...\n", tsl_gettid());
    for (int i = 0; i < 3; i++) {
        printf("[Thread %d] Yielding, iteration %d.\n", tsl_gettid(), i);
        tsl_yield(TSL_ANY);
    }
    printf("[Thread %d] Exiting...\n", tsl_gettid());
    tsl_exit();
}

int main() {
    tsl_init(1); // e.g. ALG_FCFS = 1

    int t1 = tsl_create_thread(thread_function_1, NULL);
    int t2 = tsl_create_thread(thread_function_1, NULL);
    int t3 = tsl_create_thread(thread_function_1, NULL);

    // Join all threads
    tsl_join(t1);
    tsl_join(t2);
    tsl_join(t3);

    printf("[Main] All threads have finished.\n");
    return 0;
}