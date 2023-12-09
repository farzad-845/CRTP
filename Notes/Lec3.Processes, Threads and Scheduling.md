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

#### Ready State (Computable)

- When a process is in the **Ready** state, it means that it's prepared to execute its coded instructions.
- At this point, the process is not idle; instead, it's actively awaiting the scheduler's decision to grant it access to a processor. 
- The process isn't hindered by pending I/O operations or external requests.

#### Running State

- Once the scheduler assigns a processor to a ready process, its state transitions to **Running**.
- In the Running state, the processor is entirely at the behest of the program, with the operating system taking a step back.

> **Important Note:** While a program is in execution, the control of the processor is wholly associated to it— the OS does not intervene directly.

#### Wait State

- A process enters the **Wait** state if it initiates an I/O operation or calls upon an OS function that requires waiting for completion.
- During this wait time, the process cannot utilize the processor, as it's pending the termination of an external action—like an I/O operation.

#### Interrupts and Ready State

- This "forced" transition occurs as a result of an **Interrupt**.
- An interrupt allows the OS to regain control of the processor, effectively interrupting the process's task regardless of the process's readiness or willingness to continue execution.
---
### Threads

#### The Role of Threads

- Threads represent distinct flows of control that can run concurrently within a single application.
- They are particularly useful in scenarios where multiple activities, while separate, work towards a common objective and need to collaborate closely.

#### Why Threads Over Processes?

- Unlike isolated processes, threads can naturally share resources such as memory structures and open files, streamlining cooperative tasks within an application.
- Initiating these activities as independent processes could complicate resource sharing, hence threads offer a more seamless approach.

#### Benefits of Using Threads

- **Resource Sharing**: Threads within the same process can access shared system resources effortlessly, including files, devices, and network connections.
- **Memory Sharing**: By operating in the same address space, threads can easily exchange information through shared variables, aligning with many interprocess communication mechanisms that leverage such shared resources.

#### Thread Context
---
#### Shared Resources in Thread Context

- All threads within a single process can access the same **virtual address space**, which includes components such as the page table and open files.
- This shared access facilitates communication and resource management among threads, eliminating the overhead associated with inter-process communication.

#### Thread-Specific Information

Each thread, while part of the same overarching process, contains its own exclusive set of data:

- **Processor Registers**: Including the Program Counter (PC), which dictates the next set of instructions to execute for each thread.
- **Stack Memory**: Threads maintain separate stacks to keep track of local program variables essential for their individual execution paths.

#### Shared Among Threads

Despite threads having their own unique data, they also share certain elements:

- **Global Variables**: Static variables that are accessible by all threads within the process.
- **Program Code**: The actual code of the program is shared, though each thread may execute different parts of it in practice, such as various routines and functions.

### The Thread Memory Model
---
## Local Variables on the Stack

- Every thread has its own stack, and variables that are allocated here are known as **local variables**. 
- These are typically the variables you declare within the functions or routines in your C program.

## Independence of Local Variables

- Threads can concurrently execute the same function without interfering with each other's local variables.
- This isolation ensures that the execution of routines by one thread remains unaffected by the actions of another thread.

## Shared Variables

- In contrast, **global variables** or variables declared as `static` within the C code, exist in a shared space.
- These variables are accessible to all threads within the process, facilitating a straightforward method of sharing data.

## The Double-Edged Sword of Shared Memory

- Shared memory simplifies the process of exchanging information between threads, but it introduces complexity when it comes to communication and synchronization. **BUT**, it's important to recognize that shared memory alone does not guarantee safe and efficient coordination between threads.

## Synchronization Is Key

- To prevent issues such as race conditions, where two threads attempt to modify the same variable concurrently, robust synchronization mechanisms are vital.
- Tools like mutexes, semaphores, and critical sections are often employed to manage access to shared resources, ensuring that only one thread can modify a shared variable at a time.
---
# Scheduler

#### Core Functions of the Scheduler

- **Task Selection**: The scheduler's primary role is to decide which task gains control of the CPU to proceed with execution.
- **Priority-Based Scheduling**: This decision is largely based upon task priority, with higher-priority tasks typically taking precedence over lower-priority ones.

#### When Does the Scheduler Act?

The scheduler's intervention is triggered under specific circumstances:

- **Interrupts**: When an interrupt signals a change in a process's state, for instance, if an I/O operation concludes, altering a task's status from waiting to ready.
- **OS Calls**: If a process makes a system call that could switch its state from running to waiting, such as initiating an I/O operation.

#### Post-Action Protocol

- After handling the interrupt or system call, the OS must invoke the `schedule()` function to reassess and rearrange which task should be next in line for execution.

#### Timing and Priority Dynamics

- **Timer Interrupts**: A significant trigger for the scheduler is the Timer Interrupt, typically occurring around 60 times per second (60 Hz).
- This interrupt allows the OS to adjust dynamic priorities and allocate time slices appropriately, ensuring a balanced and fair distribution of CPU time among tasks.

---
## Scheduler data structures

#### Priority-Based Queuing

- Ready tasks are arranged into **queues** according to their priority level.
- It ensures that tasks of higher priority are selected for execution before those with lower priority.

#### Time Slice Allocation

- Tasks that are not on a First-In, First-Out (FIFO) basis are given a **time slice**—a set amount of processor time.
- Time slices are implemented to guarantee fairness, allowing tasks of the same priority to get an equal opportunity to run.

#### The `scheduler_tick()` Function

- Invoked at every **timer interrupt**, `scheduler_tick()` counts down the time slice of the currently running task.
- As the time slice of a task depletes to zero, it is then moved to the **expired queue**.

#### Queue Management

- When the running task's time slice expires, `schedule()` is called, and the processor is then assigned to the next task in the **active queue**.
- If an active queue becomes empty, it swaps roles with the corresponding expired queue, ensuring continuity in the execution of tasks.

#### Efficient Priority Queue Selection

- The selection of the queue with the highest priority is optimized to be constant time, **O(1)**, using a **bitmap**.
- This data structure allows the scheduler to quickly identify and select the next eligible task without significant delay.
---
## Task Priority 

#### Understanding Task Priority Levels

- Linux defines **140 priority levels**, where a lower numerical value corresponds to a higher scheduling priority.
- Priorities range from **0 to 139**; the lower the number, the higher the priority given to the task.

#### Fixed Versus Dynamic Priorities

- Priorities from **0 to 99** are considered **fixed** and do not change during the execution of a task.
- Priorities from **100 to 139** are **dynamic** and can be adjusted by the system based on the task's "nice" value, a user-space tool to alter the priority of processes.

#### The Goal of Dynamic Priority Adjustment

- Dynamic adjustment is applied to provide a balance between resource usage and responsiveness, essentially improving fairness which translates to smoother user interaction.

#### The Role of Timer Interrupts in Priority Adjustment

- During **timer interrupts**, the system will decrease a counter associated with the running task.
- When this counter reaches zero, the priority of the task is dynamically lowered, ensuring tasks that use more CPU time gradually yield to others.

#### Increasing the Priority of Waiting Tasks

- Conversely, waiting tasks—that is, tasks that are not using the CPU—may have their priority gradually increased.
- This is to ensure that tasks which consume less CPU time are favored by the scheduler, promoting better responsiveness overall.

#### Protection Against Resource Hogging

- This priority mechanism is designed in such a way that even if a high-priority task enters an infinite loop, it won't starve the system.
- Eventually, its priority would be lowered to allow other tasks to proceed, thereby preventing a single task from completely blocking the system.
---
## Fixed vs Dynamic task priorities

#### The Essence of Dynamic Task Priorities

- **Dynamic priorities** play a crucial role in enhancing the user experience by preventing extensive blocks in task execution, particularly from CPU-intensive tasks with higher static priorities.
- The ability for the system to adjust task priority dynamically enables a more responsive interaction for the user, ensuring that no single task can monopolize the CPU indefinitely.

#### Limitations of Dynamic Prioritization

- While dynamic prioritization enhances user interaction, it cannot guarantee sustained highest priority for critical tasks.
- Consequently, a task with initially high priority may experience increased **latency** due to priority adjustments, affecting its response time.

#### Defining Task Latency

- **Latency** is defined as the duration from the moment an event makes a task ready to the point when the task actually starts executing on the processor.
- Events triggering a task's readiness might include:
  - Completion of an I/O operation.
  - Receipt of new input data.
  - Completion of a preset interval, relevant for cyclic tasks.
  - An interrupt occurrence signaling a service condition.

#### Influencers of Task Latency

Various elements can impact the latency of a task, such as:

- The presence of other tasks at an equal or higher priority which may block lower priority tasks from running.
- The quantity of available processing cores can mitigate latency; more cores mean potentially more tasks being processed simultaneously.
- The inherent **OS latency** which involves context switching, scheduling decisions, and other overheads.

#### Critical Points for Consideration

- Systems with real-time requirements might prefer fixed priorities for critical tasks to ensure predictable latencies.
- Systems that emphasize user experience might lean towards dynamic priorities for better responsiveness and fairness.
