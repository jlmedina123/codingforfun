# 2 Concurrency

* Cases (UP: uniprocessor, SMP: symmetric multiprocessing)
	1. 2 threads in proc context, UP, no preemption: no locking
	2. 2 threads in proc context, 1 in interrupt context, UP, no preemption:
		* process: local_irq_disable() 
		* ISR: no locking
	3. 2 threads in proc context, 1 in interrupt context, UP, preemption:
		* process: spin_lock_irqsave()
		* ISR: no locking
	4. processes and ISR, SMP, preemption:
		* process: spin_lock_irqsave()
		* ISR: spin_lock()
        

https://www.kernel.org/doc/Documentation/locking/spinlocks.txt

* ISRs run with IRQ disabled on ALL CPUs, so they dont need to be reentrant

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
    * multiple reades, one writer
    * requires more operations than spinlocks, so worth it only if critical section long
    * RCUs are usually preferable (eg: for list traversal)

* spin_lock
    * ok only with process context
    * if locked by process context, 
        * ok if ISR locks in another CPU: ISR will wait for process to release
        * deadlock if ISR locks in same CPU: process preempted, so it never runs 
 

* memory barriers
    * They impose a perceived partial ordering over the memory operations on either side of the barrier.
Such enforcement is important because the CPUs and other devices in a system
can use a variety of tricks to improve performance, including reordering,
deferral and combination of memory operations; speculative loads; speculative
branch prediction and various types of caching.  Memory barriers are used to
override or suppress these tricks, allowing the code to sanely control the
interaction of multiple CPUs and/or devices


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
	* struct file_operations: read, write, ioctl, open
	* struct eep_bank: struct i2c_client, struct cdev, addr
	* struct eep_bank banklist[NUMBBANKS];
	* struct i2c_driver: unique id, attach(),
	* struct i2c_client_address_data slaveaddrs 
	* add entry to /etc/udev/rules.d/
	
	1. `module_init()`: creates /dev/eep/0 .. NUMBBANKS
		* kmalloc(eep_bank * NUMBBANKS)
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

* Enable MTD device (implemented as platform device

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
	.prove = myprobe, }; 

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
	struct resource *res = pdev->resource
	request_mem_region(res->start, res->end, pdev->name);
	
	// 2) map bus address to CPU address (API pci_iomap, iomap, iomap_nocache)
	struct map_info *mymap;
	void *iomem = ioremap_nocache(res->start, res->end - res->start + 1);
	
	// 3) Populate map_info struct with data and methods
	struct map_info * mymapdriver;
	mymapdriver->virt = iomem;
	...
	
	// 4) probe flash with a MTD chip driver
	do_map_probe("cfi_probe, mymydriver);
	
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
* 
	* 
		* JFFS2: implements log structure -> power-down relaible, increase life span

	
* Support raw I/O -> flash device as character device

	
