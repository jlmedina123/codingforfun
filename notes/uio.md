
https://www.kernel.org/doc/htmldocs/uio-howto/


## About

driver parts
    * small kernel module
    * larger part of driver in user space

* Uses /dev/uioX for mmaping, and sysfs for attributes
* interrupts
    * Read blocks /dev/uioX until interrupt. Number returned is number of interrupts
    * If multiple IRQ sources but same IRQ mask and status registers, use write to /dev/uiox to call irqcontrol()
    * kernel module can also implement handler, called by uio handler
    * polled cards, set timer to trigger interrupt handler (uio_event_notify)
* attributes
    * /sys/class/uio/uiox
    * from uio framework: name, version, event
* mapping
    * /sys/class/uio/uiox/maps/mapy/: name, addr, size, offset
    * for port IO: /sys/class/uio/uiox/portio/: name, start, size, porttype
    
    
## Kernel module


* struct uio_info
    * struct uio_mem mem[MAX_UIO_MAPS]: memory to map with mmap
    * struct uio_port port[MAX_UIO_PORTS_REGIONS]: pass ioport info to userspace
    * long irq: irq number, or UIO_IRQ_CUSTOM if no irq but trigger handler, or UIO_IRQ_NONE if no irq at all


* setting uio_info in probe function:

```
struct uio_info *info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
info->mem[0].addr = pci_resource_start(dev, 0);
info->mem[0].internal_addr = pci_ioremap_bar(dev, 0);
info->mem[0].size = pci_resource_len(dev, 0);
info->mem[0].memtype = UIO_MEM_PHYS;
info->version = "0.0.1";
info->irq = dev->irq;
info->irq_flags = IRQF_SHARED;
info->handler = myhandler;

uio_register_device(&dev->dev, info);
pci_set_drvdata(dev, info);
```

* Adding interrupt handler
    * handler work in kernel
    * can buffer data for userspace to use later
    * if shared interrupts, check if device triggered the IRQ
    * if not shared interrupts, device might not work if no free IRQs

* If platform device, define struct platform_device, struct resource[] and struct uio_info in same file. platform_device.name = "uio_pdrv"

## Generic interrupt handler

* If interrupt not shared (its own line) -> use generic interrupt handler
	* struct uio_info.handler = NULL
	* struct platform_device.name = "uio_pdrv_genirq"
	* disable IRQ: disable_irq_nosync(), enable: echo 0x1 > dev

	
## Generic interrupt dynamic memory

To dynamically allocate regions in user space driver, and access DMA memory

* struct platform_device.name = "uio_dmem_genirq"
* struct platform_device.platform = struct uio_dmem_genirq_pdata 
	* struct uio_info uioinfo 	
	* unsigned int * dynamic_region_sizes: list of sizes of dynamic regions
	* unsigned int num_dynamic_regions: number of dynamic regions
* information for dynamic memory regions from /sys/class/uio/uioX/maps/mapY/*

## User space driver

* lsuio -> lsits UIO devices and attributes
* mmap() -> offset = N * getpagesize() to map memory of mapping N
* read /dev/uioX -> blocks until interrupt

## Generic PCI UIO driver

* Removes need to write hardware specific kernel module

1. Load driver, and pass device id family id

```
modprobe uio_pci_generic
echo "8086 10f5" > /sys/bus/pci/drivers/uio_pci_generic/new_id
```

2. Unbind specific driver for device (if there is one), and bind generic driver

```
echo -n 0000:00:19.0 > /sys/bus/pci/drivers/e1000e/unbind
echo -n 0000:00:19.0 > /sys/bus/pci/drivers/uio_pci_generic/bind
ls -l /sys/bus/pci/devices/0000:00:19.0/driver
```

* Interrupts
	* uses Interrupt Disable bit in PCI command register, and Interrupt Status bit in PCI status register
	* On interrupt -> uio_pci_generic sets interrupt disable bit -> userspace should clear it after processing interrupt (with pci sysfs or libpci 

## Example:

```
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
	int uiofd;
	int configfd;
	int err;
	int i;
	unsigned icount;
	unsigned char command_high;

	uiofd = open("/dev/uio0", O_RDONLY);
	if (uiofd < 0) {
		perror("uio open:");
		return errno;
	}
	configfd = open("/sys/class/uio/uio0/device/config", O_RDWR);
	if (configfd < 0) {
		perror("config open:");
		return errno;
	}

	/* Read and cache command value */
	// ssize_t pread(int fd, void *buf, size_t count, off_t offset);
	err = pread(configfd, &command_high, 1, 5);
	if (err != 1) {
		perror("command config read:");
		return errno;
	}
	command_high &= ~0x4;

	for(i = 0;; ++i) {
		/* Print out a message, for debugging. */
		if (i == 0)
			fprintf(stderr, "Started uio test driver.\n");
		else
			fprintf(stderr, "Interrupts: %d\n", icount);

		/****************************************/
		/* Here we got an interrupt from the
		   device. Do something to it. */
		/****************************************/

		/* Re-enable interrupts. */
		err = pwrite(configfd, &command_high, 1, 5);
		if (err != 1) {
			perror("config write:");
			break;
		}

		/* Wait for next interrupt. */
		err = read(uiofd, &icount, 4);
		if (err != 4) {
			perror("uio read:");
			break;
		}

	}
	return errno;
}
```
