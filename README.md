# User-Level Thread Library (Linux/x86-64)

## Overview
A custom, cooperative user-space thread management library built from scratch for Linux/x86-64 architectures. The library manages thread lifecycles, custom context switching, and stack memory allocation without relying on native POSIX threads (pthreads).

## Technical Details
* **Environment:** C, x86-64 Architecture, Linux
* **Core Features:** Implements custom TCB (Thread Control Block) structures, register state saving/restoration, and stack discipline using `getcontext()` and `setcontext()`.
* **Scheduling:** Features pluggable scheduling algorithms, including First-Come-First-Serve (FCFS) and Random selection, to manage the run queue and thread yielding.

## Build & Execution
Compiled and tested on Ubuntu 22.04 (x86-64).

```bash
# Build the static library and test applications
make

# Run the test application
./app
