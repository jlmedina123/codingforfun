# 2 Concurrency


* ISRs: run with IRQ disabled on ALL CPUs => doesnt need to be reentrant
* softirq: run with softirq disabled on local CPU => doesnt need reentrant, but protect global data => use spin_lock or per-cpu variables for global data
* timer: same as tasklet
* tasklets: run with tasklet disabled on ALL CPUs => doesnt need reentrant

* `spin_lock_irqsave`: disables ALL interrupts on LOCAL CPU (so ISR is not preempted by higher priority ISR)
* `spin_lock_bh`: disables software interrupts on local CPU, but not hardware interrupts


* SMP Linux settings
	* `CONFIG_SMP` off, `CONFIG_PREEMPT` off: spinlocks dont exist at all
	* `CONFIG_SMP` off, `CONFIG_PREEMPT` on: spin_lock_irqsave() = local_irq_disable()

	
* Cases (UP: uniprocessor, SMP: symmetric multiprocessing)
	1. 2 threads in proc context, CONFIG_SMP off, CONFIG_PREEMPT off: 
		* no locking
	2. 2 PC, UP, preemption: 
		* mutex: threads holding lock is preempted, next thread will lock and sleep. Can cause priority inversion.
	3. 2 threads in proc context, 1 in interrupt context, UP, no preemption:
		* process: `local_irq_save()`.
		* ISR: no locking
	4. 2 threads in proc context, 1 in interrupt context, UP, preemption:
		* process: `spin_lock_irqsave()`
		* ISR: no locking
	5. processes and ISR, CONFIG_SMP on, CONFIG_PREEMPT on::
		* process: `spin_lock_irqsave()`
		* ISR: `spin_lock()` // irqsave disabled interrupt on local CPU, but interrupt could happe on another CPU
	6. ISR and device, CONFIG_SMP on, CONFIG_PREEMPT on:
		* ISR: `spin_lock_irqsave()`. IRQ disabled on all CPUs, but ISR could be interrupted by higher priority ISR. During that time device might access critical section 
	7. softirq (or tasklet, timers) and process context (CONFIG_SMP on, CONFIG_PREEMPT on)
		* PC spin_lock_bh: disables softirqs on local CPU (or spin_lock_irq) 
		* softirq: spin_lock to protect from same softirq on different CPU (assuming data was global)
	8. two same softirqs (global data)
		* spin_lock, or per-CPU variables. The same softirq could be running on a different CPU
	9. two different softirqs
		* spin_lock
	8. same tasklets/timers:
		* tasklets never run concurrently on two CPU, so no need to lock
	9. two different tasklets/timers:
		* spin_lock. No need to disable tasklet (spin_lock_bh), since tasklets disable tasklet on all CPUs
	10. ISR and softirq
		* hard IRQ: spin_lock (can't be preempted by soft IRQ)
		* soft IRQ: spin_lock_irqsave (to avoid being preempted by hard IRQ)
	11. Two different ISR
		* spin_lock_irqsave  


* PC and IC, UP, no preemp -> local_irqsave, no locking
* PC and IC, UP, preemp    -> spin_lock_irqsave, no locking
* PC and IC, SMP, premmp   -> spin_lock_irqsave, spin_lock
* softirq global data, or two softirq      -> spin_lock
* softirq, ISR             -> spin_lock, spin_lock_irqsave

https://www.kernel.org/pub/linux/kernel/people/rusty/kernel-locking/c214.html

        
https://www.kernel.org/doc/Documentation/locking/spinlocks.txt

* spin lock irq disable:
    * disabled interrupts on local CPU
    * guarantee only one thread-of-control in critical section
    * works for UP and SMP

```
spinlock_t lock;
spin_lock_init(&private->lock);
unsigned long flags;
spin_lock_irqsave(&lock, flags);
// ... critical section here ..
spin_unlock_irqrestore(&lock, flags);
```
   
* read write lock irq disable
    * multiple readers, one writer
    * requires more operations than spinlocks, so worth it only if critical section long
    * RCUs are usually preferable (eg: for list traversal)

* spin_lock
    * ok only with process context
    * if locked by process context, 
        * ok if ISR locks in another CPU: ISR will wait for process to release
        * deadlock if ISR locks in same CPU: process preempted, so it never runs 
 
* memory barriers
    * They impose a perceived partial ordering over the memory operations on either side of the barrier. Such enforcement is important because the CPUs and other devices in a system can use a variety of tricks to improve performance, including reordering,
deferral and combination of memory operations; speculative loads; speculative
branch prediction and various types of caching.  Memory barriers are used to
override or suppress these tricks, allowing the code to sanely control the
interaction of multiple CPUs and/or devices
	* needed if
		* If operation affects multiple CPUs
		* atomic operations that dont imply memory barriers 
		* accessing memory mapped devices

```
#define smp_mb__before_atomic_dec() barrier()
#define smp_mb__after_atomic_dec() barrier()
#define smp_mb__before_atomic_inc() barrier()
#define smp_mb__after_atomic_inc() barrier()
 
#define barrier() __asm__ __volatile__("": : :"memory")
```

* atomic operators
	* set/read on x86: any aligned pointer, aligned long, int, or char can be used for atomic set/read as long as ACCESS_ONCE and memory barriers are used to avoid compiler optimizations  
	* Add, subtract, increment, decrease: use lock, so they dont need memory barriers 
	* set_bit, clear_bit, change_bit	
	* test_and_set_bit, test_and_clear_bit, test_and_change_bit
	* atomic_cmpxchg	

```

typedef struct {
u64 __aligned(8) counter;
} atomic64_t;
 
typedef struct {
volatile int counter;
} atomic_t;

// read and set
atomic_t = struct volatile { int counter; }
#define atomic_read(v) ((v)->counter)
#define atomic_set(v,i) (((v)->counter) = (i))

// access once
#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

void atomic_add(int i, atomic_t *v);
void atomic_sub(int i, atomic_t *v);
void atomic_inc(atomic_t *v);
void atomic_dec(atomic_t *v);
 
static inline void atomic_add(int i, atomic_t *v)
{
	asm volatile(LOCK_PREFIX "addl %1,%0"		: "+m" (v->counter)
		: "ir" (i));
}
static inline void atomic_inc(atomic_t *v)
{
	asm volatile(LOCK_PREFIX "incl %0"
		: "+m" (v->counter));
}

#define LOCK_PREFIX \
	".section .smp_locks,\"a\"\n" \
	_ASM_ALIGN "\n" \
	_ASM_PTR "661f\n" /* address */ \
	".previous\n" \
	"661:\n\tlock;

```

* seqlocks: read-write locks where writers are favored

* RCU: 
	* reader dont get blocked by writers
	* writers use callback mechanism. data update when all readers complete
	* useful when data is more read than written

		

# 6 Serial

UART: chip to implement serial communication

Layers of serial subsystem:

 1. low-level UART driver
 2. tty driver
 3. top-level line discipline
 
### UART driver

* Don't expose methods to sys calls -> they service tty driver

* UART driver structs:
	* `struct uart_driver`: minor, major, tty_driver
	* `struct uart_port`: iobase, membase, irq, uartclock, fifosize, x_char
	* `struct uart_ops`: start_tx, stop_tx, ...

* USB_UART platform device example:
	* struct uart_ops
	* struct uart_driver
	* struct uart_port
	* struct platform_device
	* struct platform_driver
	* module_init()
		* `uart_register_driver()`
		* `platform_device_register()`
		* `platform_driver_register()` -> triggers probe
	* `platform_probe()`
		* `uart_add_one_port()`: calls `uart_configure_port` -> port->ops->config_port
		* `platform_set_drvdata()`
		* `request_mem_region()` ??
	* `config_port()`
		* `port->type = PORT_USB_UART`

		
### TTY driver

`struct tty_struct` -> `struct tty_flip_buffer`
`struct tty_driver` -> defines ops open, write, put_char

### Line Disciplines	

For processing data and transferring between kernel and user
Default: N_TTY
	

# 8 I2C and serial

### I2C 

* I2C protocol: 
	* master-slave, multidrop, between host controller and client slave 
	* 2 wires: SCL, SDA
	* 7-bit addresses: 127 bus on bus
	* SMBus is subset of I2C (all SMBus cmds are supported by I2C)
* Bus transactions:
	* Write: S slave-addr  Wr-cmd  [A]  offset [A] data [A] P   
	* Read: S slave-addr Wr [A] 0 [A] S slave-addr Rd-cmd [A] [data] NA P

* I2C EEPROM with one char interface per bank
	* `struct file_operations`: read, write, ioctl, open
	* `struct eep_bank`: struct i2c_client, struct cdev, addr
	* `struct eep_bank banklist[NUMBBANKS]`;
	* `struct i2c_driver`: unique id, attach(),
	* `struct i2c_client_address_data slaveaddrs` 
	* add entry to /etc/udev/rules.d/
	
	1. `module_init()`: creates /dev/eep/0 .. NUMBBANKS
		* `kmalloc(eep_bank * NUMBBANKS)`
		* `alloc_chardev_region(&devnumber, 0, NUMBBANKS`
		* `class_create()`: create sysfs entry
		* for eebank in eebanks { 
			* `cdev_init(&eebank.cdev, fops, )`: connect fops
			* `cdev_add(&eebank.cdev, minor)`: add cdev to char system
			* `device_create(eppclass, , devnumber, NULL, )`: create node
		* `i2c_add_driver(i2c_driver)`: registers attach probe
	
	2. `open(inode, file)`: store private data in file for access late: `file->private_data = banklist[minor]`
	
	3. `i2c_driver.attach_adapter()`: called by core when it finds host controller
		* `i2c_probe(adapter, &slaveaddrs, epp_attach)`: pass slave address and attach function. Host controller walks bus and looks for slave with slaveaddrs. transaction S ADDR Wr. Slave responds [A] -> calls epp_attach() for each slave found
	
	4. `epp_attach()`: registers client data structure  
		* struct i2c_client *client = kmalloc
		* client->addr = ...
		* i2c_attach_client(client)

	5. read
	
```C
ssize_t read(struct file *file, char *buf, size_t count, loff_t *p) {
	// 1) get private data
	struct ee_bank * bank = file->private_data
	// 2) check if func supported
	i2c_check_functionality(bank->client, I2C_FUNC_SMBUS_READ)
	// 3) read
	mybuff[i++] = i2c_smbus_read_word_data(bank->client
	// 4) copy to user buffer
	copy_to_user(buffer, mybuffer, bytes) 

```

### SPI: serial pheripheral interface

* master-slave
* 4 wires: SCLK, CS, MOSI, MISO
* full duplex
* ~MHz range 

# 17 MTD


##MTD Subsystem

* MTD core
* Map drivers
* NOR chip drivers
* NAND chip drivers
* User modules


## Map driver

* Enable MTD device (implemented as platform device)

```
// 1) define partitions
static struct mtd_partition mypartitions[] {
	.name = ,
	.size = ,
   .offset = ,
   .mask_flags = , }, { ... },};

// 2) define resources
struct resource myresource = {
	.start = ,
	.end = ,
	.flags = IORESOURCE_MEM,};
	
// 3) define platform device
struct platform_device myplatdev = {
	.name = "name",
	.resource = &myresource, };

// 4) define platform driver
struct platform_driver myplatdriver = {
	.driver = { .name = "name", },
	.probe = myprobe, }; 

static int __init module_init() {
	
	// 5) register platform device
	platform_device_register(&myplatdev); 
	
	// 6) register platform driver. Since it matches
	// platform device, probe will be called
	platform_driver_register(&mydriver);

}
```

* probe method

```
static int myprobe(struct platform_device *pdev) {

	// 1) request (reserves) memory region (so other drivers dont use it)
	struct resource *res = pdev->resource // equivalent to pci_resource_start(bar)
	request_mem_region(res->start, res->end, pdev->name);
	
	// 2) map bus address to CPU address (API pci_iomap, iomap, iomap_nocache)
	struct map_info *mymap;
	void *iomem = ioremap_nocache(res->start, res->end - res->start + 1);
	
	// 3) Populate map_info struct with data and methods
	struct map_info * mymapdriver;
	mymapdriver->virt = iomem;
	...
	
	// 4) probe flash with a MTD chip driver
	do_map_probe((char *)"cfi_probe", (struct *map_info)mymapdriver); // cfi_probe, jedec_probe, map_rom, map_ram, ...
	
	// 5) register MTD partition
	struct mtd_info * mymtd;
	add_mtd_partitions(mymtd, mypartitions, <numpart>);
	
	// /dev/mtd/0, ... should show up
	
```

NOR chip drivers

* most implement specifications like CFI and JEDEC
* command-sets for compliants chips supported as kernel modules (enable in kconfig)


NAND chip drivers

* USB pen drive: controller emulates	 SCSI storage interface -> don't need NAND driver to communicate
* On-board NAND flash chips -> needs special drivers
	* Not connected to CPU via data/address lines -> uses flash controller 
	* CPU read flash -> CPU issues read command to NAND controller -> controller copies from NAND to small internal RAM
	* NAND flash chips dont support CFI
	* inform MTD about NAND chip properties -> `nand_flash_ids[]`
	* faults very common -> spare area in each flash page with OOB information
	* OOB includes ECC for error correction -> specify via struct nand_ecclayout according to chip's properties

``` 
struct nand_ecclayout {
	uint32_t eccbytes; 	  // numb of bytes that store ECC data
	uint32_t eccpos[64];  // offset to ECC data within OOB area
	uint32_t oobavail;    // unused bytes in OOB area
	struct   nand_oobfree oobfree[MAX]; 
}
```

* MTD to implement error managemenet software (if NAND controller hardware doesnt support it) 

## User modules

* Support file I/O -> flash device as disk (block device)
	* __mtdblock__: emulates harddisk -> `/dev/mtdblock/<part>`
	* __NFTL__: like mtdblock but with wear leveling -> `/dev/nftl/<part>`
	* Create filesystem:
		* ext2 
		
```
$ mkfs -t ext2 /dev/mtdblock/2
$ mount /dev/mtdblock/2 /mnt
```


	
