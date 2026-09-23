#define _GNU_SOURCE
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>

// Direct access to general-purpose registers using sigcontext
void print_context_info(ucontext_t *uc) {
    struct sigcontext *sc = (struct sigcontext *) &uc->uc_mcontext;

    printf("RIP (Instruction Pointer): 0x%llx\n", (unsigned long long) sc->rip);
    printf("RSP (Stack Pointer):       0x%llx\n", (unsigned long long) sc->rsp);
    printf("RBP (Base Pointer):        0x%llx\n", (unsigned long long) sc->rbp);
    printf("RDI:                       0x%llx\n", (unsigned long long) sc->rdi);
    printf("RSI:                       0x%llx\n", (unsigned long long) sc->rsi);
    printf("RAX:                       0x%llx\n", (unsigned long long) sc->rax);
    printf("RBX:                       0x%llx\n", (unsigned long long) sc->rbx);
    printf("RCX:                       0x%llx\n", (unsigned long long) sc->rcx);
    printf("RDX:                       0x%llx\n", (unsigned long long) sc->rdx);
    printf("R8:                        0x%llx\n", (unsigned long long) sc->r8);
    printf("R9:                        0x%llx\n", (unsigned long long) sc->r9);
    printf("R10:                       0x%llx\n", (unsigned long long) sc->r10);
    printf("R11:                       0x%llx\n", (unsigned long long) sc->r11);
}

int main() {
    static int first_time = 1;
    ucontext_t ctx;

    if (getcontext(&ctx) == -1) {
        perror("getcontext failed");
        exit(1);
    }

    printf("\n=== Inside main() ===\n");

    if (first_time) {
        first_time = 0;
        printf("Calling print_context_info() after getcontext():\n");
        print_context_info(&ctx);

        printf("Jumping to saved context using setcontext()...\n\n");
        if (setcontext(&ctx) == -1) {
            perror("setcontext failed");
            exit(1);
        }
    } else {
        printf("===> Returned from setcontext(), execution resumes here!\n");
    }

    return 0;
}
