# Operating Systems – Laboratory Assignments

This repository contains five laboratory assignments from an Operating Systems course.

The labs focus on Linux programming, threads, synchronization, inter-process communication, scheduling, and memory management.

---

## Lab 1 – Linux, Threads and Message Queues

This lab introduces the Linux programming interface and basic operating system concepts.

The work includes creating threads with `pthread`, using mutexes, and communicating between threads through message queues. The lab also introduces wrapper functions around Linux system calls that are reused in later assignments. :chatgpt-content-reference{index="0"}

### Learning

- Linux API
- Processes and threads
- `pthread`
- Mutexes and critical sections
- Message queues
- Inter-process communication
- Passing data between threads
- Using structs in message queues
- Synchronization basics

---

## Lab 2 – Synchronization

This lab focuses on mutual exclusion and synchronization using mutexes and semaphores.

The first part studies the Producer-Consumer problem and how incorrect synchronization can cause resource contention. The second part implements the Dining Philosophers problem and focuses on preventing deadlock. :chatgpt-content-reference{index="1"}

### Learning

- Mutexes
- Counting semaphores
- Shared resources
- Resource contention
- Producer-Consumer problem
- Dining Philosophers problem
- Deadlocks
- Thread synchronization
- Safe access to shared data

---

## Lab 3 – Messaging, Threads and Client-Server Communication

This lab focuses on processes, threads, inter-process communication and shared resources in Linux.

A planetary system is implemented using a client-server model. Each planet is handled by its own thread, while message queues are used for communication between clients and the server. :chatgpt-content-reference{index="2"}

The server keeps track of active planets and updates their positions, while clients can create planets and receive messages when planets die or leave the simulation area. :chatgpt-content-reference{index="3"}

### Learning

- Client-server architecture
- Threads and processes
- IPC
- POSIX message queues
- Shared resources
- Mutex protection
- Thread-per-object design
- Linux system programming
- Concurrent simulation
- Graphical output with GTK

---

## Lab 4 – CPU Scheduling

This lab focuses on how an operating system schedules tasks.

A simulated operating system is used to study task states, ready queues and scheduling policies. The assignment extends a Round Robin scheduler with additional scheduling strategies. :chatgpt-content-reference{index="4"}

The implemented scheduling policies include:

- Round Robin
- Shortest Job First
- Multiple Queues with priorities

The Multiple Queue scheduler uses different time quanta depending on task priority. :chatgpt-content-reference{index="5"}

### Learning

- CPU scheduling
- Round Robin
- Shortest Job First
- Priority scheduling
- Multiple Queue scheduling
- Ready and waiting queues
- Task states
- Time quantum
- Scheduler design

---

## Lab 5 – Page Replacement Algorithms

This lab focuses on virtual memory and page replacement algorithms used by the Memory Management Unit.

The goal is to study how an operating system decides which page should be removed from physical memory when a new page must be loaded. :chatgpt-content-reference{index="6"}

The algorithms studied and implemented include:

- LRU – Least Recently Used
- FIFO – First In First Out
- LFU – Least Frequently Used
- OPT – Optimal Page Replacement, or another chosen algorithm :chatgpt-content-reference{index="7"}

### Learning

- Virtual memory
- Pages and frames
- Page faults
- Page replacement
- FIFO
- LFU
- LRU
- OPT
- Memory management
- Comparing replacement strategies

---

## Main Technologies and Concepts

- C
- Linux
- POSIX Threads
- Mutexes
- Semaphores
- Message Queues
- Inter-Process Communication
- GTK
- CPU Scheduling
- Virtual Memory
- Memory Management

## Author

**Hadel Othmany**

Computer Science  
Mälardalen University
