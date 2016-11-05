
# 1. Intro

* Elements of embedded Linux
    * toolchain
    * bootloader
    * kernel
    * root filesystem: libraries and programs to run


# 2. Toolchains

* Toolchain:
	* tools that compiles source code into executable for the target device
	* includes compiler, linker, libraries
	* needed for the bootloader, kernel, root filesystem
	* Components: 
		* binutils: assembler, linker
		* gcc
		* C library
* CPU arch: toolchain build for a particular one
	* architecture
	* big/little endian
	* floating point support, or library
	* ABI
* C libraries: glibc, eglibc, uclibc, musl libc 
* Create cross development toolchain: crosstool-ng
	* select closest target configuration: ./ct-ng arm-crtex...
	* review config: ./ct-ng menuconfig -> creates .config
	* 	 ./ct-ng build -> creates xx-gcc, sysroot with libraries
* C library components:
	* libc, libm, libpthread, librt
	* libc always linked, other add -l <m/thread/rt> 
* static linking: `xx-gcc -static mylib.c -o mylib-static`
* creating static library: create archive of object files

```
xx-gcc -c lib1.c
xx-gcc -c lib2.c
xx-ar ar libtest.a lib1.o lib2.o
xx-gc myprogram.c -ltest -L../libs -I../libs -o mypgogram
```

* shared library: library shared object linked at runtime

# 3. Bootloaders

After power-up, memory not accessible -> bootloader initializes system and loads kernel

* Boot sequence with linear addressing (NOR flash): simply map flash into address space at reset vector (0xFF FF FF FC)

* Phases:
  1. ROM code loads small code from SRAM. But SRAM not large enough for full booloader, so it needs Secondary Program Loader
  2. SPL initializes memory, loads Third Program Loaderon DRAM, and jumps to it
  3. TPL: bootloader is loaded

* Bootloader to kernel: pass system information -> device tree
* U-Boot
	* Build: make CROSS_COMPILE=xx_defconfig
	* boot ROM can read u-boot image from USB, sd card, serial, etc...
	* load kernel image
	* run kernel image: bootm [addr kernel] [addr ramdisk] [addr dtb]

# 4. Kernel	 

Build kernel

```
$ cd /usr/src/
$ wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.9.3.tar.xz
$ tar -xvJf linux-3.9.3.tar.xz
$ cd linux-3.9.3
$ make menuconfig
$ make
$ make module
$ make modules_install
$ make install
```

Creates files:

* vmlinuz-3.9.3 – The actual kernel
* System.map-3.9.3 – The symbols exported by the kernel
* initrd.img-3.9.3 – initrd image is temporary root * file system used during boot process
* config-3.9.3 – The kernel configuration file

BusyBox: performs all essential functions needed by init

# 7. Storage

* NOR flash
	*  expensive, reliable, low capacity
	*  can be mapped into CPU addresses, so that CPU can execute code directly from it
	*  memory cells arranged in erase blocks
	*  It can be programmed one word at a time
	*  Can be read word by word
* NAND flash
	* 	cheaper, high capacity, more complex
	*  one bit per cell SCL, two bits MLS, three TLC
	*  Number of erase cycles before degradation is low
	*  Can be read or written only in pages -> cant be mapped to address space
	*  prone to bit flips -> uses ECC and OOB data per page
* Managed flash  
	* NAND with a hardware interface
	* Eg: USB flash, SD card
* MTD
	* Architecture
		* chip driver (NOR, NAND)
		* mtd core, 
		* block device (/dev/mtdblockN) 
		* char device (/dev/mtdN): includes ioctls. mtd-utils  program uses it
	* partitions:
		* via kernel cmd line
		* device tree
		* platform mapping driver: struct mtd_partition
* Filesystems for flash
	* FTL: adds suballocation, garbage collection, wear leveling, bad block handling, robustness
* Filesystems for NOR and NAND flash
	* JFFS2
	* YAFFS2
	* UBIFS   
	 
	 
# Device drivers

* list block and char drivers: `cat /proc/devices`
* list interfaces: `ip link show`
* devices attached to bus: `lspci`, `lsusb`
* Devices discovered:
	* sys devices: `ls /sys/devices/system`
	* virtual devices (memory based): `ls /sys/devices/virtual`
	* platform devices: `ls /sys/devices/platform`


### User-space drivers:

* GPIO
	* kernel subsystem gpiolib -> creates /sys/class/gpio
	* export GPIO to control it: `echo <gpionum> > /sys/class/gpio/export`
	* `cd /sys/class/gpio/gpiox`
		* control direction: `echo [in|out] > direction`
		* read gpio (in): `cat value`
		* write gpio (out): `echo [0|1] > value`
		* generate interrupts: `echo [rising|falling|both|none] > edge`      
		* wait for interrupts:

```
struct pollfd poll_fds[1];
fd = open("/sys/class/gpio/gpiox", O_RDONLY);
poll_fds[0].fd = fd;
poll_fds[0].events = POLLPRI | POLLERR;
char value[4];
while (1) {
	poll(poll_fds, 1, -1);
	read(fd, &value, sizeof(value));		
	printf("Button pressed: read %d bytes, value %c\n",
			  n, value[0]);
}
```

* LEDs
	* controlled via GPIO or with led kernel subsystem
	* `ls /sys/class/leds/<device:color:function>`
		* brightness
		* trigger: events that trigger LED to turn on

* I2C
	* 2-wire bus, master-slave protocol, 
	* for devices not in the SoC
	* SMBus is subset of I2C 
	* One device node for each host controller: `ls /dev/i2c*`
	* Access slave from user-space:
		* i2c-tools 
		* i2c-dev.h (comes from i2c-tools package)


```bash
$ yum install i2c-tools
$ i2c
i2cdetect  i2cget  i2c-stub-from-dump  i2cdump i2cset       
$ i2cdetect -l
$ i2cget <bus> <chip-addr> <data-addr>
```


	

```c
// 1) Open host controller
int fd = open("/dev/i2c-1", O_RDWR);
// 2) Specify slave device addr to communicate to
ioctl(file, I2C_SLAVE, addr);
// 3) Read with i2c-dev.h library
__s32 res = i2c_smbus_read_word_data(fd, 0x10)
// Write  with i2c-dev.h library
i2c_smbus_write_word_data(file, 0x10, 0x6543);
// read with syscall
read(fd, buf, 1)
// write with write syscall
char buf[3];
buf[0] = 0x10; // device register
buf[1] = 0x43;
buf[2] = 0x65;
write(fd, buf, 3);
```

https://www.kernel.org/doc/Documentation/i2c/dev-interface

* SPI
	* faster than I2C, 4 wires, separate send/receive (full duplex), chip select line, master-slave protocol
	* generic SPI kernel driver: `/dev/spi<bus>.<chipselect>`

### Character device interfaces

* ioctl
* sysfs
* mmap
* sigio
* debugfs: `mount -t debugfs debug /sys/kernel/debug`
* proc
* netlink socket
	
 ### Discovering hardware
 
* PCI and USB: have selfdicoverable capabilities
* Discovering SoC hardware:
   	* device tree
 	* platform_device: includes array of resources

Device tree example:

```
net@xxxx {
	compatible = "name";
	reg = <0x100000100 0x10000>;
	inerrupts = <25>;
};
```

Platform data example:

```
static struct resource dev_resources[] = {
	{ .start = BASE,
	  .end = BASE + SIZE,
	  .flags = IORESOURCE_MEM,
	 }, {
	  .start = IRQ_DEV,
	  .end = IRQ_DEV,
	  .flags = IORESOURCE_IRQ,
	  }, 
}; 
static struct platform_device dev {
	.name = DEV,
	.id	   = 0,
	.num_resources = 2,
	.resource = dev_resources,
};

```

Device driver example:

```
static const struct of_device_id dev_match[] {
	 { .compatible = "smsc,dev", },
	 {},
};
 
static struct platform_driver dev_driver = {
	.probe = myprobe,
	.driver = {
		.name = "name",
		.of_match_table = of_match_ptr(dev_match),
	},
	
static int __intit driver_init(void) {
	return platform_driver_register(&dev_match);
}
```

* device configured by device tree: kernel matches `of_device_if[x].compatible` to device tree compatible
* device confihgured by platform data: kernel matches `of_device_if[x].compatible` to `driver.name`


probe function
```
static int myprobe(struct platform_device *pdev) {
	...
	// 1)get memory and IRQ from either device tree or
	// from platform data
	res = platform_get_resources(pdev, IORESOURCE_MEM, 0);
	platform_get_resources(pdev, IORESOURCE_IRQ, 0);
	// 2) map memory
	ioremap(res->start, SMC_IO_EXTENT);
	// 3) install IRQ
	...
}
```



mtrace
	


	

	       	