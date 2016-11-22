
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
