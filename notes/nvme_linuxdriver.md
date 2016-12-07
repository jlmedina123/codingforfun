



## NVME protocol

http://www.nvmexpress.org/wp-content/uploads/2013/04/NVM_whitepaper.pdf

* interface tha provides optimized command issue and completion path
* support parallel operation with 64K command queues within an I/O queue
* doesnt require MMIO register reads
* max of one MMIO register write necessary
* support 64K IO queues with 64K commands each
* all information for 4KB read request in 64B command
* registers
    * Controller Capabilities register
    * status
    * admin queue configuration
    * doorbell register for scalable number of submission and completion queues
    
* submission and completion queues paired up (submissions queue can share completion queue, though)
* commands placed in submission queue (host memory)
* completions placed in associated completion queue (host memory)
* admin submission and completion queue for device management and control (admin command set)
* I/O command set
* host software creates submission/completion queue pairs (usually one pair per CPU to avoid locking and improve caching)

* I/O Submit Queue:
    * circular buffer to submit commands for controller execution
    * host updates sq tail doorbell register when there are commands in queue  
    * controller fetches sq entries (commands)

* Commands:
    * 64 bytes
    * memory location of command specified in Physical Region Page entries
    * each command up to two PRP entries 

* I/O Completion Queue
    * circular buffer to post status for completed commands
    * command identified by SQ identifier and command identifier
    * CQ head pointer updated by host indicating last free CQ entry


http://www.flashmemorysummit.com/English/Collaterals/Proceedings/2013/20130812_PreConfD_Marks.pdf

* queues
    * located in host memory
    * contiguous physical memory, or non-contiguous pages (PRP List)
    * circular
        * tail: points to next free element
        * head: points to next entry to be pulled off (if queue not empty)
    * types
        * admin queue for admin command sets: one per controller
        * I/O queue for IO command sets: up to 64K queues, 64K elements per queue
    * each type:
        * submission queue: to submit commands
        * completion queue: to post command completions
        * may have msi-x interrupt per completion queue

* command execution 
    * queue command in submission queue
    * ring door with new tail
    * fetch and process command, queue completion, generate interrupt
    * process completion
    * ring doorbell with new head
* nvme controller can support weighted round robin with priority class arbitration

* nvme subystem:
    * controller: pci function
    * namespaces: nvm storage formatted for block access
    * namespace ID: identifier for NS

* some registers
    * capabilities, version, 
    * interrupt mask set/clear
    * admin submission/completion queue base address
    * IO submission queue i-th tail doorbell
    * IO completion queue i-th head doorbell
   
* Submission Queue Element with PRP (64B command)
    * Opcode, Command identifier, namespace
    * PRP (physical Region Page): Memory page address, and offset within page

* Submission Queue Element with SGL
    * Opcode, Command identifier, namespace
    * SGL (Scatter Gatther List): list of SGL descriptors
 
* Completion Queue Element (16 B)
    * SQ header pointer, SQ identifier, command identifier, phase tag, status field 
 


## NVME driver


http://www.flashmemorysummit.com/English/Collaterals/Proceedings/2013/20130812_PreConfD_Busch.pdf



* nvme driver:
    * register pci driver
    * pci enable
    * select bars
    * io remap
    * dma settings
    * mem pools
    * IRQ
    * MSI
    * MSI-x

* IO stack 
    * block layer -> `make_request_fn` -> `nvme_make_request()` 
        * select NVME IO queue for currently running CPU
        * allocate command ID
        * allocate resources to build command
        * IO flags
            * Read/Write
                * map BIO vector to PRP
                    * No PRP
