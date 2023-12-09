### Process Abstraction Overview

In computing, **process abstraction** serves as the fundamental mechanism to facilitate concurrent execution, particularly within the confines of a **single processor** environment.

### Concurrent Execution in Modern Machines

Modern machines typically boast multi-core processors, representing an intricate blend of genuine parallel execution and orchestrated management by the operating system (OS).

### The Lifecycle of a Running Program

A running program is dynamically characterized by its state of information, which the OS is tasked to preserve through timely snapshots. These snapshots are essential, especially when the program must relinquish control of the processor.

#### What is Process Context?

The concept of process context extends beyond merely the program itself, encompassing:

- All memory content managed by the program, such as the program stack and global variables.
- The current state of the processor's registers.
- Any OS resources actively utilized by the process.

Crucially, one must understand that **process context is distinct from interrupt context**.

---

### Process Control Block (PCB)

The **PCB** plays a pivotal role by housing the saved state of registers and OS resources within a specific data structure under OS jurisdiction.

- Direct copying of a process's memory content is not feasible.
- It is **IMPERATIVE** for physical memory locations to remain insulated from the process's perceived memory locations.
- The Page Table Entries stored in the PCB encompass several critical components:
	- Program Code
	- Program Stack for local variables
	- Static memory for static and global variables
	- Dynamically allocated memory managed by the heap.

The process's speed can be impacted by both the **volume of information transferred** and the **extent to which the process is engaged** with substantial memory utilization.

It is also worth noting that the **TLB (Translation Look-aside Buffer)** necessitates purging with each context switch.

---

## Ensuring Memory Protection through Virtualization

- Utilization of virtual memory is paramount in safeguarding against incorrect memory allocations.
- In the absence of virtual memory, user programs could potentially compromise the integrity of other processes or the OS's data structures.
- The virtue of virtual memory allows processes to be assigned their own memory area (_PTE_)📝, enabling unfettered operation within this designated space, where all access is deemed legitimate.

> 📝 **Note on Memory Access:** Whenever a process seeks to retrieve data from its memory, the OS is responsible for translating the virtual address given by the process into the physical address where the actual data resides. This mapping takes place within the page table, with each correlation termed a _page table entry_ (PTE).

---
### The Process States
