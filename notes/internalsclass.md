# Linux notifiers

* Signals: for user processes, sync or async
* hardware interrupts: async, kernel runs interrupt handler
* request_irq: driver registers handler for IRQ number
 irq_desc_ptrs[IRQ no] -> struct irq_desc->(struct irqaction *)action->handler, next (if line shared)
* exceptions: sync, due to some error
* trap: cause change in operation mode of system. eg: trap to switch from user mode to kernel mode

# Process management

* task_struct
    * related tasks:
        1. pid_t pid
        2. struct list_head thread_group -> list of threads within process
        3. struct list_head children -> list of all children
        4. struct list_head siblings -> list of all children from same parent
        5. struct task_struct *parent ->all tasks in siblings list point to same parent
    * struct cred *cred -> process credentials. User user_struct has struct cred. Task points to the user’s struct cred.
        1. uid (user id), gid (group id),
        2. (struct group_info *)group_info
        3. (struct user_struct *)user: processes, files, etc
    * signal management:
        1. struct signal_struct *signal -> for management of signals
rlim[]: resource limits for CPU, file size, data memory, stack, etc
number of context switches, page faults, etc
        2. struct sighand_struct *sighand -> array of signal handlers registered by   task
        3. struct sigpending pending -> signals pending (received)
    * files:
        1. `struct files_struct *files` -> structure with file descriptor table, which has entries with pointers to files opened
        2. `struct fdtable __rcu *fdt;`
        3. `struct file __rcu **fd;      /* current fd array */`
        4. `struct file __rcu * fd_array[NR_OPEN_DEFAULT];`
        5. `struct fs_struct *fs` -> file system information, such as pwd
    * memory management:
        1. struct mm_struct *mm -> memory management
        2. void *stack -> points to bottom of kernel stack associated with the user task
    * scheduler:
        1. struct sched_class *sched_class
        2. struct sched_entity se
        3. struct sched_rt_entity rt
        4. uint rt_priority
    * struct thread_struct thread

* per-task kernel stack
	* 2 pages (8KB) for 32-bit, 8 pages (32KB) for 64-bit system
	* stack pointer register (ESP): points to bottom of the stack (stack grows downward)
	* struct thread_info at bottom of stack
		* struct task_struct *task ->points to user task
	* (used to be) shared with interrupt handler

* current:
	* ia64: gets process’ task_struct from general purpose register 13
	* powerpwc: from R2

* jiffies, jiffies_64, get_jiffies_64()

* fork
	* files, fs, sighand, cred, mm: child has its own copies for struct pointed to. But data is copied, so structs are identical to parent’s ones
	* share text (until exec)
	* share data, stack (until written)
	* same CPU registers (keep running same line of code)

* pthread_create
	* thread pointers files, fs, sighand, mm point to struct of parent


# System calls

* mode switch
	* old: interrupt or trap 0x80 -> calls system_call()
	* new: instruction SYSENTER -> calls sysentre_entry()
* every syscall has an assigned number
* sys_call_ptr_t sys_call_table[__NR_syscall_max+1]: pointers to syscall routines.  Eg: sys_read, sys_write, etc
* flow:
	1. application calls open from library
	2. library: save args and syscall no in CPU registers (eax syscall no, ebx arg1, ecx arg2, etc), executes SYSENTER
	3. kernel: sysenter_entry() gets syscall no from eax > index to sycall_table -> sys_routine()

reference to read: https://lwn.net/Articles/604287/

# Memory management

* 32-bit virtual addr space division
	* 0x0 – 0xc0 00 00 00: (3GB) user space
	* 0xc0 00 00 00 – 0xFF FF FF FF: (1GB) kernel space
* Zones 32 bits:
	* 0 - 16MB: DMA
	* 16MB - 896MB: normal
	* 896MB - 1MB: reserved for mapping high memory to kernel
	* 1MB - 4GB: high. since kernel virtual addr is physical+3GB, a addr in this area would have kernel virtual addr > 4GB -> cannot be mapped by kernel, used by applications only  
* Zones 64-bit:
	* zone_dma (0-16MB): old ISA devices
	* zone normal (16MB – ): for kernel and user space. kernel can map because there is no 4GB limit, so it can create kernel virtual addresses for it
* Types of addresses:
	* Physical addr: used between CPU and main memory
	* User virtual addr: used by process
	* Bus address: used between peripheral device and main memory. Same as physical in x86 without IOMMU. Platforms with IOMMU can remap addresses
	* Kernel logical address: kernel memory from normal zone. Mapped linearly as physical + PAGE_OFFSET. kmalloc returns this address
	* kernel virtual address: memory from high zone mapped to kernel. Uses page table for non-linear translation. vmalloc retusn this address

* Memory architecture
	* UMA: all CPUs have consistent view of memory -> all memory a single chunk
	* NUMA: memory and CPU more tightly linked -> chunks of memory -> nodes


### Kernel memory allocation:

* kmalloc:
	* allocates pinned contiguous physical memory, mapped to contiguous kernel virtual addresses, from kernel’s heap
	* virtual addr returned is physical addr + PAGE_OFFSET ( 1:1 logical mapping)
	* 128KB limit per allocation
	* not page aligned
	* buddy system to avoid fragmentation: hash bucket linked lists of free memory in power of 2s (32B to 128 KB) -> min 32 B and max 128 KB. use __get_free_pages for more. Can waste up to 50% memory
	* GFP_KERNEL:  `(__GFP_WAIT | __GFP_IO | __GFP_FS)` -> can sleep and reschedule (for paging out if needed)
	* GFP_ATOMIC: `(__GFP_HIGH)` -> access emergency pools
	* implementation: kmalloc ->__do_kmalloc
		* cachep = kmalloc_slab(size, flags): finds kmem_cache structure for the size
			* index = size_index[size_index_elem(size)];
			* return kmalloc_caches[index];
		* void *ret = slab_alloc(cachep, flags, caller);

* `get_free_page`: one single page, page aligned,

* `__get_free_pages`: allocate 2^arg pages (from 2^0 to 2^11 = 2048 pages = 8MB)

* slab cache/allocator: for allocation sizes that are needed frequently. Keeps pools of slabs.
	* `kmem_cache_t * kmem_cache_create()`: create pool of slabs
	* `kmem_cache_alloc(kmem_cache_t * cache, flags)`: allocate slab of memory

* vmalloc:
	* allocates pageable memory from kernel’s virtual address space
memory from VMALLOC_START to VMALLOC_END range
	* can’t be used outside of processor (such as DMA, PCI device), because they need MMU translation
	* Needs to build page tables for the memory allocated (like ioremap, but with allocation). Kernel has single large page table entry on top of kernel region for this non-logical mapping, needed if memory > 1 GB.
	* Only for software buffers used only by CPU
	* Causes less fragmentation, so recommended for large CPU buffers

* mem_pool: allocation is not allowed to fail, from pool of memory 
* alloc_bootmem: large contiguous allocation at boot-time
* per-cpu variable:
	* each processor gets independent copy, avoid locking (unless process moved to different processor, or preempted within critical section
	* user space requests values -> sum values for all processors
	* dynamic: void *alloc_percpu(type), per_cpu_ptr(), put_cpu()
	* static: define_per_cpu(), get_cpu_var(), put_cpu_var()
* others (dont allocate):
	* `ioremap(), ioremap_nocache(), pci_iomap()`: for mmio. maps bus memory into CPU space. It makes bus memory accessible to CPU via readb/l/w and writeb/l/w. Maps (it creates PTE entries) device memory to kernel virtual addr space. Address returned cannot be used directly deferenced by CPU. Use wrappers write/readb/w/l()
	* `request_mem_region`: for mmio. to reserver range of physical addresses that device maps and uses
	* `remap_pfn_range`: for mmap. reserves virtual address range and maps it to range of physical pages that were previously allocated. Usually used for mmap implementation. Allows direct access to device memory or kernel memory from user space. Creates kernel page table entry that points main memory or device memory to virtual address space memory

### node:

* chunk of memory (cell) associated with a CPU
* cache coherent NUMA: any processor attached to any cell can access any memory cell, cache coherency handled by hardware. Access time by processor attached to cell less than to remote cell
* each node -> Linux builds independent memory management subsystem
* local allocation: linux will try to perform memory allocation for the node from that CPU
represented by struct pglist_data
	* node_id: 0 always for UMA
	* node_start_pfn: physical page number of first memory frame (0 for UMA)
	* node_present_pages: numb pages in node
	* node_zones: array with zones for the node
* watermarks to wake up kswapd if low in memory
* mem_map[]: array of struc page types for each physical page

### Virtual address space:

* 32-bit process image:
	* (kernel), stack, (lib suse), gap, heap, data/BSS, text, (lib RH), memory mapped files
* malloc:
	* size from heap: raises break point of application’s data memory mapping. requested + size (4bytes), *prev malloc (4 bytes)
	* size >128KB -> anonymous private pages allocated by kernel and added as VMA section to the process address space
* thread stacks
	* each under previous stack, default 10MB upper limit
	* red zone pages: page between threads’ stacks, with no access bits set, to avoid stack overflow

* 64-bit process: 8 regions: 3 kernel, data/stacks, text, shared memory, 32-bit emulation
* current->mm: struct mm_struct: manages address space of task
	* `struct vm_area_struct *mmap`, `struct rb_root mm_rb`  -> to list of VMAs: describes layout of memory region
		* vm_start, vm_end, vm_flags
		* vm_file -> if VMA for memory mapped file, points to entry in kernel’s file table entry associated with file
		* vm_pgoff: page count within file wheren mapping begins
		* vm_next, vm_rb: point to list of VMAs for that task (ordered by address and in a hash chain based of Red/Black tree)
	* mmap_cache -> last VMA used, for find_vma(). if miss, traverse mm_rb
	* pgd_t * pgd: pointer to page table for process
	* indexes for regions: start/end_code, start_end_data, start_brk, brk, start_stack, etc

* address translation 32-bits
	* cpu switches to process -> it loads  current->mm->pgd to cr3 register, which loads page tables and flushes TLB
	* linear address: 10 bits PDI, 10 bits PTI, 12 bits page offset (0-4095 -> 4 KB page size)
		* Page (Global) Directory: 10 bits -> 2^10=1024 entries, each entry a 4 bytes address -> point to PMD
		* Page Middle Directory: for 32-bit all PGD point to same PMD (one PMD per process)
		* Page Table: 32-bits entries:
			* 20 bits for page frame number
			* 12 bits flags:
 				* `_PAGE_PRESENT (LSB)`:
					* 1 = page located where 20 bits point to.
					* 0 = page not in memory  -> page fault
						* 23 bits: swap device offset, 8 bits: type of paging space device
						* if 31 bits NULL: page not allocated yet. Eg: when malloc is called
				* `_PAGE_ACCESSED`: to check if page is being used, if not swap it out if needed
				* `_PAGE_DIRTY`: on when contents of page in memory have been modified and do no match contents of the page out on disk. Page must be written back to disk

* address translation 32-bit PAE
	* linear address: 2 bits PGDT, 9 bits PGD, 9 bits PT, 12 bits page offset
	* tables 64 bits: PGD pointer table, 4 Page Global Directories, Page Tables

* address translation 64 bits
	* linear address: PGD 10 bits, reserved 21 bits, PMD 10 bits, PTE 10 bits, page offset 13 bits
		* Page Directory, Page Middle Directories, Page Tables

* Kernel High Memory mapping (32 bit):
	* Low memory permanently mapped (+PAGE_OFFSET), but not high memory (if memory > 1GB)
	* page in PTE reserved to map 1024 high pages into low memory via kmap
	* last 128 MB of kernel addr space used for temporary mapping of high-memory page frames
	* techniques for mapping:
		* kmap: maps high memory permanently to low memory
		* kmap_atomic: temporary: mapped through window in kernel virtual addr space (page table entry reserved)
		* vmalloc: allocates physically non-contiguous memory. Maps allocated pages into contiguous kernel virtual space
		* ioremap: maps bus memory into CPU space. It makes bus memory accessible to CPU via readb/l/w and writeb/l/w. Maps (it creates PTE entries) device memory to kernel virtual addr space
		* request_mem_region: to reserve range of physical addresses that device maps and uses
		* remap_pfn_range: reserves virtual address range and maps it to range of physical pages that were previously allocated. Usually used for mmap implementation. Allows direct access to device memory or kernel memory from user space

```
vmalloc()
	__vmalloc_node_range(VMALLOC_START, VMALLOC_END…)
		area = __get_vm_area_node()
		addr = __vmalloc_area_node(area,..)
	page = alloc_pages_node(node, tmp_mask, order);
	map_vm_area(area, prot, &pages)
```

		 
* Anonymous mapping:
	* memory mapping with no file or device backing it (heap, stack, etc)
	* initially only allocates virtual memory (no physical) -> it only adds page table 	* entries, and maps virtual address to (physical) zero page
	* physical memory allocated on-demand, from page faults
	* access unmapped memory (without page table entry) generates SIGSEGV
	* Linux leaves first KB of process virtual address space unmapped, so deferencing null pointer tries to access unmapped memory, and generates SIGSEGV.

* File backed mapping:
	* mirrors the content of an existing file
	* MAP_SHARED: a write updates page cache  immediately (other processes see it)
	* MAP_PRIVATE: a write performs COW and allocates new local copy of page

* Page fault: virtual addr doesn’t have entry in page table, or physical page is not allocated, or its’s allocated but paged out, or can’t write to it. 
	* Cases:
		* _PAGE_PRESENT bit off, frame available -> page not in physical memory, it was paged out
		* _PAGE_PRESENT bit off, frame NULL -> page not allocated. eg: malloc
		* _PAGE_PRESENT bit on, _PAGE_RW bit off, when trying to write to page -> allocate new page. eg: COW
	* page fault handler: exception -> do_page_fault
		* get faulting address from CPU
		* find_VMA()
			* faulting address above task’s VMA -> SIGSEV if user mode -> termination of task
			* faulting add below -> check if VMA can grow downward -> if it cannot cover faulting addr -> SIGSEV
			* if within valid VMA -> check permissions on page.
				* if page missing -> handle_mm_fault

# Inside Disk-Based File System

UNIX file types
regular files
directories
special files: character/block devices: /dev/name, they have M/m numbers
sockets: no /dev/name, no M/m numbers, they use interfaces
FIFO
symbolic links
Regular files
no structure, no end-of-file character
stored in 4KB data blocks -> wastes space for small files, more efficient I/O on larger files
data blocks can be contiguous or fragmented -> can degrade performance
each file (of any type) -> one inode
i_block[] : array of block numbers, or extents, where data is
i_rdev: major and minor number for the case of special device
each inode an unique number -> filesystem has inode table
directory: table to convert filename into inode number
file system
inode table -> file refered by inode number
identify file:  M/m number->disk partition, inode numb -> file within disk
directory
type of file, but with a structure -> slots with filename and inode number
identify file -> pwd from task_struct for task, filename for directory in that path -> namei()
basic filesystem
implemented in disk partition
block 0: boot block, block 1: superblock, 2-n: inode table, n+1-z: data blocks
superblock
number of inodes, number of blocks, block size, … -> tune2fs
journaling filesystem
Journaling Block Device layer
transaction within I/O operation is performed in memory -> logged by journal into memory buffer -> written to journal on disk
all transactions completed in memory and logged by journal -> commit record in log -> atomic state
sync operation (write in memory to disk) -> sync record to log
boot-up, if journal has transaction record without sync -> system crashed -> perform operations that have commit in log
ext4
block 0: boot block
block groups: 6 components each
copy of superblock
block group descriptor: struct ext4_group_desc
two bit maps: for data blocks and inodes
data blocks and indirect blocks
block based fs (ext2, ext3)
inode -> array i_block[]: 15 elements:
0-11 pointers to data blocks (48 KB max)
12 ptr -> single indirect block -> array of 1024 block data pointers: max 4MB file
13 ptr-> double indirect block -> 1024 SIBs: max 4GB file
14 ptr -> triple indirect block -> 1024 DIBs: max 2TB file
extent-based: inode has extent descriptors -> array of contiguous data blocks


# Kernel Filesystem Structures

file descriptors
current->(struct files_struct *)files->fd_array
syscalls open/dup/socket -> assigns one file descriptor to fd_array; pipe -> assigns two fd
stdin, stdout, stderr
si=open(“/dev/tty7”, O_RDONLY) or si=open(“/dev/tty7”, O_RDWR);
so=open(/dev/tty7″, WRONLY) or so=dup(si)
se=dup(si)
fd_array = [si, so, se]
each entry in fd_array points to opened file represented by struct file
f_list netx, prev
f_op
f_pos
memory-mapped file I/O
open file and access it -> file’s pages loaded into kernel disk cache buffer  -> copied into user’s buffer -> overhead
mmap: file’s pages are mapped to the task’s page table, and a VMA is added task’s address space
mmap returns starting addr where file mapped -> use it as ptr to array of bytes in memory
 flags:
MAP_ANONYMOUS flag creates anonymous mappings
MAP_SHARED: a write updates page cache immediately (other processes see it)
MAP_PRIVATE: a write performs COW and allocates new local copy of page
Direct IO: 
I/O operations buffered though kernel-space buffer
I/O directly to user-space buffer (opposite of mmap) -> improves performance for large amounts of data
no_pages = get_user_pages(current, current->mm, …, pages)
if no DMA: kmap each page
if DMA: create s/g list from pages array: dma_map_sg(…, sg, …)
if pages written: setPageDirty(page)
page_cache_release(page)
1
2
3
4
5
6
7
8
9
// user space  process mapping file:
int fd = open("somefile", ORDWR);
char * fp = mmap(NULL, len, PROT_READ, flags, fd, 0);
for (i = 0; i&amp;amp;lt;len; i++) printf("%c", *fp++); // driver performing direct I/O: down_read(current-&amp;amp;gt;mm-&amp;amp;gt;mmap_sem);
result = get_user_pages(current, current-&amp;amp;gt;mm, ...);
up_read(current-&amp;amp;gt;mm-&amp;amp;gt;mmap_sem);
...
if (! PageReserved(page))
SetPageDirty(page);
Signal management

32 posix-defined signals
signal number -> index into bit mask
kill syscall -> sends signal to PID
async signal -> by another process or hardware, to all threads of process (will terminate process anyway)
sync signal -> sent as result something the process did (eg: SIGSEGV)
signal delivery:
posted to bit mask at: task_struct-> (struct sigpending) pending ->(sigset_t) signal
bit mask checked for pending signals when process returns to user mode
if signal sent to task sleeping in TASK_INTERRUPTIBLE, sleep cancelled and task returns to user mode
actions by the process when signal received
nothing -> process accepts signal and default action occurs
ignore it ->signal lost
block it ->delay delivery
catch/trap/handle signal -> routine will execute (SIGKILL/SIGSTOP cannot be caught)
sigaction syscall -> handle signal
Device I/O

character device: I/O one character at a time, file type ‘c’
block device: I/O through buffer in chunks, for random access, part of filesystem, file type ‘b’
device loaded:
character: cdev_add() -> struct cdev to cdev list with name of device and pointer to device’s file operations
block: gendisk_register() -> struct gendisk added to list
device for special file: ‘mknod /dev/mydev type(c,b,p) M n’ -> creates node, and stores M/n (same as device driver)
int fd = open(“/dev/rmt0”, O_RDWR)
 1) copy filename from user space
getname(filename)
strncpy_from_user()
2) get first unused file descriptor (will be returned to process)
int fd = get_unused_fd()
struct files_struct *files = current->files
3a) get file from filesystem
struct file *f = file_open(filename)
open_namei
lookup operation for filesystem: dentry = cached_lookup or real_lookup
initializes file struct: dentry_open
3b) if it is special device (character or block)
find inode
get M/m numbers from inode->i_rdev
M/m no -> dev_t -> search in cdev list (character) or gendisk list (block)
cdev struct -> points to file_operations, which go to file->f_ops
4) install file returned by filesystem into file descriptor table for the process
fd_install: current->files->fd[fd] = file
control device -> R/W to its registers at addresses in memory addr space or I/O addr space
difference between I/O register and RAM: side effects that affect I/O registers
cache data -> use volatile variable declaration
instruction reordering -> use memory barriers
I/O Ports
some arch have separate R/W electrical lines and special CPU instr to access ports -> separate addr space (only x86)
request_region: claim port
access port: [in/out][b/w/l], [in/out]s[b/w/l]
I/O Memory:
memory mapped registers: device exposes registers through bus as a memory region
ioremap: when page tables needed (depends on arch)
claim memory region: request_mem_region
access memory: io[read/write][8/16/32], [read/write][b/w/l], do not dereference pointers (due to portability)
Linux Sockets: Type of file used to connect and end-point from a process to another end-point from another process, which can be on another system Address family

INET (internet protocol)
UNIX
AppleTalk
Types sockets

Connection sockets: TCP
server
fd = socket(inet, stream (TCP), …)
bind(): associates socket to port number known by applications
listen()
accept
client
socket()
connect(IP addr, port for application): forms virtual circuit
read(fd)/write(fd)
Connectionless sockets: UDP
server/client: socket(), bind(), recvfrom(fd), sendto(fd)
struct file -> private_data -> struct socket

ops
sk_buff: associated with a data packet sent/received
head, data, tail, end
11) Linux Process Scheduling

performance: response time and system throughoutput
real time: for time-critical applications, system must response within guaranteed predictable time
support RT: adjustable priorities, fast context switching, controllable time slices, preemtable kernel
Scheduling policies:
real-time:
FIFO: no time slice, process runs until complete if it has highest priority
RR: tasks same priority, task at head gets time slice, if used task back to list.
non real-time:
NORMAL: for regular processes
BATCH: for CPU-bound processes, longer time-slices, less context switches
IDLE: runs when there is nothing else to do
Linux schedulers
goodness scheduler -> too slow
O(1) scheduler
Completely Fair Scheduler
one runqueue for each processor
runqueues arranged as R/B binary tree (balanced binary hash) according to longest waiting time -> task at left-most position has longest wait-time and is selected
virtual wait time =  real time/ number of tasks (time task would run if all tasks same priority)
scheduler runs at each clock tick (1000 times/sec) and end of interrupt
Scheduling classes (task_struct->policy)
policy NORMAL -> CFS class
priority value same for all tasks with same nice value -> nice and wait time determine favored level
FIFO or RR -> RT class
priority assigned by programmer (setpriority() )
task_struct->sched_class: pointers to functions for managing run queues (fixed by policy)
CPU affinity: task_struct->cpus_allowed
nice value: -20 to 20, default 10
priorities:
0 most favored, 139 least favored
RT: 0-99, policy normal 100-139
dynamic prority:
