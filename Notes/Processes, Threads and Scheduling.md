`process abstraction` is the `base mechanism` to allow concurrent execution (In the `single processor`)

Modern Machine host multicore = mix of true parallel and OS managed

Running program has a set of information that change over time `->` Snapshot of associated informations must be saved by OS (`WHEN ?` when the program `loses` processor ownership)

Process Context:
	Program + other memory contents manage by program (program stack and global variables)
	Current Value of the processor registers
	The OS resources currently used by process

Process context `!=` Interrupt context

---
### How to save memory contents

Process Control Block (PCB): Registers and OS resources are saved in data structure owned by the OS called PCB

- We cannot copy the content of the memory used by process.
- It's `NECESSARY` that physical memory location are independent from memory location as seen by the process.
- Page Table Entries are saved in PCB
	- Program Code
	- Program Stack for local Variables
	- Static memory for static and global variables
	- Dynamically allocated memory by heap

Speed of the process can be effected by `amount of information that copied` + `how significant is the process` using a large memory

TLB (Translation Look-aside Buffer) must `flushed` upon context switch

---
### Memory Protection

- Virtual memory usage `ensures` protection against wrong memory allocation
- Without Virtual memory, `user programs` may` harm` `other process` or `OS data structures`
- Using Virtual Memory, we give `memory area (PTE)`📝 to the process `to do whatever` it wants, and any access within this area is legal.

> `📝` When a process requests access to data in its memory, it is the responsibility of the operating system to map the virtual address provided by the process to the physical address of the actual memory where that data is stored. The page table is where the operating system stores its mappings of virtual addresses to physical addresses, with each mapping also known as a _page table entry_ (PTE)

<div class="warning" style='background-color:#E9D8FD; color: #69337A; border-left: solid #805AD5 4px; border-radius: 4px; padding:0.7em;'>
<span>
<p style='margin-top:1em; text-align:center'>
<b>PTE</b></p>
<p style='margin-left:1em;'>
When a process requests access to data in its memory, it is the responsibility of the operating system to map the virtual address provided by the process to the physical address of the actual memory where that data is stored. The page table is where the operating system stores its mappings of virtual addresses to physical addresses, with each mapping also known as a _page table entry_ (PTE)<br><br>
</p>
<p style='margin-bottom:1em; margin-right:1em; text-align:right; font-family:Georgia'> <b>- Wikipedia</b> <i>https://en.wikipedia.org/wiki/Page_table</i>
</p></span>
</div>
