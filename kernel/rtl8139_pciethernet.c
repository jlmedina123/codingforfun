#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/irq.h>
#include <linux/string.h>
#include <linux/interrupt.h>
 
#include "rtl8139.h"
 
 
// Needed for a realtek bug when writing to MMIO
#define IOWRITE8_F(reg, val8)  do { iowrite8((val8), ioaddr + (reg));\
                                    ioread8(ioaddr + (reg)); } while(0)
#define IOWRITE16_F(reg, val16) do { iowrite16((val16), ioaddr + (reg));\
                                     ioread16(ioaddr + (reg)); } while(0)
#define IOWRITE32_F(reg, val32) do { iowrite32((val32), ioaddr + (reg)); \
                                     ioread32(ioaddr + (reg)); } while(0)                                   

/* 
 *  PCI addressable regions:
 *  - configuration space
 *  - I/O ports
 *  - device memory
 *  There are 6 I/O or memory regions, addresses can be read from BAR 0-5
 *
 *  Access to regions:
 *  - Config space: 
 *	    pci_read_config_byte(pdev, offset), pci_write_config_byte(pdev, offset)
 *  - Query I/O region: 
 *	    pci_resource_start/end/flags(pcidev, bar), 
 *	    request_mem_region(ulong start, ulong len, char *name) if flags & 
 *      request_region(uint first, ulong n, const char *name)
 *      or
 *      pci_request_region(pdev, bar): checks if port io or mem io via pci_resource_flags, 
 *                                     pci_resource_start, pci_resource_len, 
 *                                     and request_mem_region(start, len, name) or 
 *                                     request_region(start, len, name)
 *  - memory-map I/O region:
 *      pci_iomap(pdev, bar, maxlen): calls pci_resource_start, pci_resource_len, pci_resource_flags
 *                                    if flags & IORESOURCE_CACHEABLE: ioremap(start, len)
 *                                    else: ioremap_nocache(start, len)
 *      iowrite8/16/32(data, ioaddr, reg)
 *      data = ioread8/16/32(ioaddr, reg) 
 *  - port-map I/O region:
 *      data = inl(iobase, REGISTEROFFSET)
 *      outl(data, iobase, REGISTEROFFSET)
 * 
 *  DMA: capability to transfer data from device to memory without CPU intervention
 *  - initiated by DMA master. DMA controller in south bridge, or PCI device can initiate DMA transfers.
 *  - must ensure CPU cache coherency
 *  - synchronous (eg: process writes to LCD), async (eg: NIC)
 *  - 24-bit buses like ISA can only access DMA buffers in bottom 16 MB
 *  - if PCI bus 32-bit wide, DMA buffer in bottom 4GB
 *  - check bus width with pci_set_dma_mask(pdev, DMA_64/32BIT_MASK)
 *  - consistent/coherent: guarantees consistency between memory and CPU cache. Used when 
 *    CPU and device need to frequently manipulate buffer. Also used with DMA descriptors
 *      + pci_alloc_consistent()
 *  - streaming: doesnt guarantee cache coherency. when device owns buffer for long period of 
 *    time. Common when each DMA operation on different buffer. Driver has to map buffer for device
 *    and then unmap it for CPU to use
 *      + pci_map_single(): single DMA buffer
 *      + pci_map_sg(): list of scatter-gather DMA buffers
 *  - Bounce buffers: buffers in DMA region used temporarily if DMA is requested from non-DMA region
 *  - If DMA data scattered over discontiguous regions, sg buffers gathers the contents of the scattered
 *    buffers in one DMA request
 */


// read https://www.kernel.org/doc/Documentation/DMA-API-HOWTO.txt


/*
static int __pci_request_region(struct pci_dev *pdev, int bar,
    if (pci_resource_flags(pdev, bar) & IORESOURCE_IO) {
           request_region(pci_resource_start(pdev, bar), pci_resource_len(pdev, bar), res_name))
    else (pci_resource_flags(pdev, bar) & IORESOURCE_MEM) {
           __request_mem_region(pci_resource_start(pdev, bar), pci_resource_len(pdev, bar), res_name, exclusive))
*/

/*
 * PCI entry points prototypes
 */
static int my_pci_probe(struct pci_dev *, const struct pci_device_id *);
static void my_pci_remove(struct pci_dev *);
 
/* 
 * network device operations
 */
static int netdev_open(struct net_device *);
static int netdev_stop(struct net_device *);
static int netdev_start_xmit(struct sk_buff *, struct net_device *);
static struct net_device_stats * netdev_get_stats(struct net_device *);
static irqreturn_t interrupt_method(int irq, void *dev_instance);
 
/*
 * NIC hardware operations
 */
static void hardware_start(struct net_device *);
static int reset_chip(void *addr);
 
/*
 * Structures used
 */
static struct pci_device_id pci_id_table[] = {
    {VENDOR_ID, DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, PCI_ANY_ID},
    { 0, }
};
 
static struct pci_driver my_pci_driver = {
    .name       = DRV_NAME,
    .id_table   = pci_id_table,
    .probe      = my_pci_probe,
    .remove     = my_pci_remove,
#ifdef PM
    .driver.pm  = rtl8169_pm_ops,
#endif 
};
 
static struct net_device_ops netops = {
    .ndo_open   = netdev_open,
    .ndo_stop   = netdev_stop,
    .ndo_get_stats  = netdev_get_stats,
    .ndo_start_xmit = netdev_start_xmit,
};
 
struct netdev_private {
    struct pci_dev *pdev;
    void *ioaddr;
    unsigned long mmio_start, mmio_end, mmio_len;
    struct net_device_stats stats;
    spinlock_t lock;
     
    struct tasklet_struct tasklet;
 
    // tx data
    dma_addr_t tx_bufs_dma; // bus addr (for device) (typedef unsigned long long dma_addr_t)
    unsigned char *tx_bufs; // virtual kernel addr (for CPU)
    unsigned char *tx_buf[NUM_TX_DESC];
     
    unsigned int cur_tx;
    unsigned int dirty_tx;
     
    // rx data 
    dma_addr_t rx_buf_dma; // bus addr
    void *rx_ring;         // virtual kernel addr
     
    unsigned int cur_rx;
};
 
MODULE_DEVICE_TABLE(pci, pci_id_table); // expose table id to pci hotplug module
 
 
 
 
/*
 * module load/unload
 */
#ifdef OLD
static int my_init(void) {
    printk(KERN_ALERT DRV_NAME ": initializing module\n");
    if (pci_register_driver(&my_pci_driver) < 0) {
        printk(KERN_ALERT DRV_NAME ": pci_register_driver failed\n");
    } else {
        printk(KERN_ALERT DRV_NAME ": pci driver registered\n");
    }
    return 0;
}
static void my_exit(void) {
    printk(KERN_ALERT DRV_NAME ": uninitializing module\n");    
    pci_unregister_driver(&my_pci_driver);
    printk(KERN_ALERT DRV_NAME ": pci driver unregistered\n");
}
#else
module_pci_driver(my_pci_driver);
#endif
 
/*
 * PCI entry points 
 */
static int my_pci_probe(struct pci_dev *my_pci_dev, 
                        const struct pci_device_id *id) {
 
    // to access PCI config and MMIO space 
    unsigned long mmio_start, mmio_end, mmio_len, mmio_flags;
    void __iomem *ioaddr;
 
    // for network device
    struct net_device *my_net_dev = NULL;
    struct netdev_private *private = NULL;
 
    int rval;
    u8 byte;
    int i;
 
    printk(KERN_ALERT DRV_NAME ": pci probe called.\n");
    
    /*
     * PCI system
     */

    // 1) Enable PCI device 
    rval = pci_enable_device(my_pci_dev);
    if (rval) {
        printk(KERN_ALERT DRV_NAME ": pci device enabled failed");
        return rval;
    }
 
    // 2) Enable bus mastering by pci device 
    pci_set_master(my_pci_dev);
    /*  pci_write_config_word(dev, PCI_COMMAND=0x4, old_cmd | PCI_COMMAND_MASTER); */
         
    // 3) Access one of the six PCI I/O regions, indicated by BAR number 
    mmio_start = pci_resource_start(my_pci_dev, 1);	// ((dev)->resource[(bar)].start)
    mmio_len =   pci_resource_len(my_pci_dev, 1);
    mmio_end =   pci_resource_end(my_pci_dev, 1);
    mmio_flags = pci_resource_flags(my_pci_dev, 1);
 
    // 4) check region is memory-mapped IO or port-mapped IO
    if (mmio_flags & IORESOURCE_IO) {
        printk(KERN_ALERT DRV_NAME ": PCI IO region 1 is port mapped\n");
        goto disable;
    }
 
    // 5) take ownership of region (ensures you have exclusive access to region)
    request_mem_region(mmio_start, mmio_len, DRV_NAME); // or pci_request_regions (pdev, DRV_NAME);
 
    // 6) maps IO region. Creates virtual mapping to PCI BAR 
    // Returns __iomem address to device BAR number given
    // It checks if region MMIO or PMIO, and calls pci_ioport_map() or ioremap() 
    // Address can be used with iowrite and ioread funcs. They hide details of MM/PMIO
    // ioremap: assigns kernel virtual addr to device io memory region
    ioaddr = pci_iomap(my_pci_dev, 1, mmio_len);  // ioremap(start, len);
 
    // 7) Set consistent (coherent) 32bit DMA
    rval = pci_set_consistent_dma_mask(my_pci_dev, DMA_BIT_MASK(32));
    /* if (!dma_supported(dev, mask))
                return -EIO;
        dma_check_mask(dev, mask);
        dev->coherent_dma_mask = mask;
    */
    if (rval) {
        printk(KERN_ALERT DRV_NAME ": couldnt set 32 bit DMA\n");
        goto unmap;
    }
     
    /*
     * Networking system
     */
    // 1) Allocate ethernet device
    my_net_dev = alloc_etherdev(sizeof(struct netdev_private));
    if (my_net_dev == NULL) {
        printk(KERN_ALERT DRV_NAME ": couldnt allocate ethernet device\n");
        goto unmap;
    }
 
    // 3) Initialize net_device structure
    // 3.1) set name
    memcpy(my_net_dev->name, DRV_NAME, strlen(DRV_NAME)+1);
    // 3.2) set netdev_ops
    my_net_dev->netdev_ops = &netops;
    // 3.3) MAC address
    // Accessing MMIO to get MAC address (first 6 bytes of MMIO region 1)
    for (i = 0; i < 6; i++) {
        byte = ioread8(ioaddr+i);
        my_net_dev->dev_addr[i] = byte;
        my_net_dev->broadcast[i] = 0xff;
    }
    // 3.4) IRQ number
    // Read assigned IRQ from PCI config space
    pci_read_config_byte(my_pci_dev, PCI_INTERRUPT_LINE, &byte);
    my_net_dev->irq = byte;
 
    // 3.5) Save net_device into pci_dev structure
    pci_set_drvdata(my_pci_dev, my_net_dev);
     
    // 4) initialize private data structure
    // 4.1) get private data structure from struct net_device
    private = netdev_priv(my_net_dev);
 
    // 4.2) Initialize and fill up data in private data structure
    spin_lock_init(&private->lock);
    private->pdev = my_pci_dev;
    private->ioaddr = ioaddr;
      
     
    // 5) Create sysfs class
    SET_NETDEV_DEV(my_net_dev, &my_pci_dev->dev);
 
    // 6) Register net device
    register_netdev(my_net_dev);
 
 
    return 0;
 
unmap:
    pci_iounmap(my_pci_dev, ioaddr);
 
//release:
    pci_release_regions(my_pci_dev);
 
disable:
    pci_disable_device(my_pci_dev);
 
    return -1;
}
 
static void my_pci_remove(struct pci_dev *my_pci_dev) {
 
    struct net_device *net_dev;
    struct netdev_private *private;
     
    printk(KERN_ALERT DRV_NAME ": pci remove called.\n");
     
    // 1) get net_device and netdev_private
    net_dev = pci_get_drvdata(my_pci_dev); // get net_dev from pci_dev
    private = netdev_priv(net_dev);     // get private from net_dev
    if (net_dev == NULL) 
        printk(KERN_ALERT DRV_NAME ": couldnt get net_dev\n");
    if (private == NULL)
        printk(KERN_ALERT DRV_NAME ": couldnt get private\n");
         
    // 2) Free net_device
    free_netdev(net_dev);
     
    // 3) Free pci_dev
    // 3.1) Unmap MMIO region
    pci_iounmap(my_pci_dev, private->ioaddr);
    // 3.2) Release region
    pci_release_regions(my_pci_dev);
    // 3.3) Set pci_dev drvdata to NULL (net_dev was released)
    pci_set_drvdata(my_pci_dev, NULL);
    // 3.4) Disable device
    pci_disable_device(my_pci_dev);
}
 
/*
 * Network device operations
 */
static int netdev_open(struct net_device *net_dev) {
    int rval, i;
    struct netdev_private *priv;
    void *cpu_addr;
    dma_addr_t pcidev_addr;
     
    printk(KERN_ALERT DRV_NAME ": netdev_open called\n");
         
    priv = netdev_priv(net_dev);
     
    // 1) Request IRQ
    /* 
    pci_enable_msi(pdev) -> __pci_enable_msi_range(dev, 1, 1, NULL);
    nr_alloc = pci_alloc_irq_vectors(pdev, min, max, typeflag); -> __pci_enable_msi_range(dev, min, max, affd);
    	-> nvec = pci_msi_vec_count(); msi_capability_init(nvec, aff);
    */
    rval = request_irq(net_dev->irq, interrupt_method, IRQF_SHARED, 
                       net_dev->name, net_dev);
    if (rval != 0) {
        printk(KERN_ALERT DRV_NAME ": request_irq failed\n");
        return rval;
    }
     
    // 2) Allocate consistent DMA buffers
    // 2.1) Receiver (ring buffer)
    // pci_alloc_consistent:
    //      paddr = alloc_pages_exact() -> __get_free_pages(): allocates pages and gets physical addr
    //      kvaddr = ioremap_nocache((unsigned long)paddr, size): gets kernel virtual address
    //      dma_handle = plat_kernel_addr_to_dma(dev, paddr): gets bus address (platform dependent)
    // returns kvaddr  and dma_handle (bus addr)     
    cpu_addr = pci_alloc_consistent(priv->pdev,
                                    TOTAL_RX_BUF_SIZE,
                                    &pcidev_addr);
    priv->rx_ring = cpu_addr; // used by CPU
    priv->rx_buf_dma = pcidev_addr; // used by device
     
    // 2.2) Transmitter (4 buffers)
    cpu_addr = pci_alloc_consistent(priv->pdev,
                                    TOTAL_TX_BUF_SIZE,
                                    &pcidev_addr);
    priv->tx_bufs = cpu_addr; // used by CPU
    priv->tx_bufs_dma = pcidev_addr; // used by device                         
                          
    // 3) Initialize buffers
    // 3.1) Receiver (1 ring buffer)
    priv->cur_rx = 0;          
    // 3.2) Transmitter (4 liner buffers)
    priv->dirty_tx = 0;
    priv->cur_tx = 0;
    for (i = 0; i < NUM_TX_DESC; i++) 
        priv->tx_buf[i] = cpu_addr + i*TX_BUF_SIZE; 
 
    // 4) Initialize hardware
    hardware_start(net_dev);
     
    // 5) Notify network layer to start
    netif_start_queue(net_dev);
                            
    return 0;
}
 
// called with command 'ifconfig MY_REALTEK8139 down'
static int netdev_stop(struct net_device *net_dev) {
    struct netdev_private *priv;
    void *ioaddr;
    unsigned long flags;
         
    printk(KERN_ALERT DRV_NAME ": netdev_stop\n");
     
    priv = netdev_priv(net_dev);
    ioaddr = priv->ioaddr;
     
    // 1) Notify network layer to stop queueing frames
    netif_stop_queue(net_dev);
     
    spin_lock_irqsave(&priv->lock, flags);
    wmb();
     
    // 2) Disable Tx and Rx (write 0 to bit  and 3)
    iowrite8(ioread8(CR+ioaddr) & ~(CmdTxEnb|CmdRxEnb), CR+ioaddr);
 
    // 3) Disable interrupts
    iowrite16(INT_CLEAR, IMR+ioaddr);
     
    spin_unlock_irqrestore(&priv->lock, flags);
     
    // 4) free IRQ
    free_irq(net_dev->irq, net_dev->name);
     
    // 5) Free DMA buffers
    pci_free_consistent(priv->pdev, TOTAL_TX_BUF_SIZE, 
                       priv->tx_bufs, priv->tx_bufs_dma);
    pci_free_consistent(priv->pdev, TOTAL_RX_BUF_SIZE, 
                       priv->rx_ring, priv->rx_buf_dma);
                        
    return 0;
}
 
// called to send a packet to device
static int netdev_start_xmit(struct sk_buff *skb, struct net_device *net_dev) {
    struct netdev_private *priv;
    void *ioaddr;
    unsigned long flags;
    unsigned int entry;
     
    printk(KERN_ALERT DRV_NAME ": netdev_start_xmit\n");
     
    // 1) Get private and mapped ioaddr
    priv = netdev_priv(net_dev);
    ioaddr = priv->ioaddr;
     
    // 2) Get next available transmit descriptor
    entry = priv->cur_tx % NUM_TX_DESC;
     
    // 3) Discard oversized packet
    if (skb->len > TX_BUF_SIZE) {
        kfree_skb(skb);
        priv->stats.tx_dropped++;
        return 0;
    }
     
    // 4) Copy sk buffer data to DMA buffer
    memcpy(priv->tx_buf[entry], skb->data, skb->len);
     
    // 5) Free sk buffer
    kfree_skb(skb);
     
    // 6) Inform device packet ready for transmit
    // Write len of packet to Transmit Status Descriptor
    spin_lock_irqsave(&priv->lock, flags);
    wmb();
    iowrite32(max(skb->len, (unsigned int)ETH_MIN_LEN), 
              TSD0 + (entry * sizeof(u32)) + ioaddr);
     
    // 7) Adjust cur_tx pointer
    priv->cur_tx++; //dirty_tx is adjustd in interrupt handler
     
    // 8) Stop network layer if out of free descriptors
    if (priv->cur_tx == priv->dirty_tx) {
        netif_stop_queue(net_dev);
    }
     
    spin_unlock_irqrestore(&priv->lock, flags);
     
    return 0;
         
}
 
static void hardware_start(struct net_device *net_dev) {
 
    struct netdev_private *priv;
    void *ioaddr;
 
    printk(KERN_ALERT DRV_NAME ": netdev_hardware_start\n");
     
    // 1) get mapped ioaddr
    priv = netdev_priv(net_dev);
    ioaddr = priv->ioaddr;
     
    // 2) reset chip
    if (reset_chip(ioaddr) != 0) {
        return;
    }
     
    // 3) Enable Tx/Rx with flsuh
    IOWRITE8_F((ioread8(CR+ioaddr) & ChipCmdClear) | CmdTxEnb | CmdRxEnb, CR);
     
    // 4) Tx config: update Transmit Config Register
    IOWRITE32_F(6 << TCR_DMA_BURST_SHIFT, TCR); // PCI burst 6 (1024)
     
    // 5) Rx config: update Receiver Config Register
    IOWRITE32_F((1 << RCR_RBLEN_SHIFT) | // Rx bufer size 32K + 16bytes
              (7 << RCR_MXDMA_SHIFT) | // DMA unlimited
              (1 << RCR_WRAP_SHIFT)  | // continue after end of Rx buffer
              (1 << RCR_AB_SHIFT) |    // AB=1: accept broadcast packets   
              (1 << RCR_AM_SHIFT) |    // AM=1: accept multicast packets
              (1 << RCR_AAP_SHIFT),   // AAP=1: accept packets with phys dest addr 
               RCR);
                
     // 6) Config Transmit Start Adr Descriptor Registers with the DMA buffer addresses
     IOWRITE32_F(priv->tx_bufs_dma + (priv->tx_buf[0] - priv->tx_bufs), TSAD0);
     IOWRITE32_F(priv->tx_bufs_dma + (priv->tx_buf[1] - priv->tx_bufs), TSAD1);
     IOWRITE32_F(priv->tx_bufs_dma + (priv->tx_buf[2] - priv->tx_bufs), TSAD2);
     IOWRITE32_F(priv->tx_bufs_dma + (priv->tx_buf[3] - priv->tx_bufs), TSAD3);
      
     // 7) Config Receive Buffer Start Addr Register
     IOWRITE32_F(priv->rx_buf_dma, RBSTART);
      
     // 8) Reset missed packet counter
     iowrite32(0x0, MPC + ioaddr);
      
     // 9) Enable interrupts
     iowrite16(ioread16(MULINT + ioaddr) & 0xF000, MULINT + ioaddr); // avoid early interr
     IOWRITE16_F(INT_MASK, IMR);
 
}
 
static int reset_chip(void *ioaddr) {
    int i;
    printk(KERN_ALERT DRV_NAME ": reset chip\n");
    iowrite8(CmdReset, CR + ioaddr);
    for (i = 0; i < 1000; i++) {
        barrier();
        if ((ioread8(CR + ioaddr) & CmdReset) == 0)
            return 0;
        udelay(10);
    }
    printk(KERN_ALERT DRV_NAME ": failed to reset chip\n");
    return -1;
}
     
static struct net_device_stats * netdev_get_stats(struct net_device *dev) {
    struct netdev_private * private = netdev_priv(dev);
    printk(KERN_ALERT DRV_NAME ": netdev_get_stats\n");
    return (&private->stats);
}
 
static irqreturn_t interrupt_method(int irq, void *dev_instance)
{
    struct net_device *net_dev;
    struct netdev_private *priv;
    void *ioaddr;
    unsigned int txstatus;
    unsigned short isr;
     
    net_dev = (struct net_device *)dev_instance;
    priv = netdev_priv(net_dev);
    ioaddr = priv->ioaddr;
     
    printk(KERN_ALERT DRV_NAME ": interrupt generated\n");
     
    // 1) Acknowdlege interrupt (read ISR and clear it)
    isr = ioread16(ISR + ioaddr);
    iowrite16(0xFF, ISR + ioaddr);
      
     // 2) Check for unknown int or error
     if (unlikely(isr & INT_MASK) == 0)
        goto out;
     if (unlikely(isr == 0xffff))
        goto out;
         
     // Transmit interrupt: packet(s) has been sent out or dropped
     //                     Clear dirty buffer
     if ((isr & TxOK) || (isr & TxErr)) {
        // Check all not serviced buffers (dirty)
        while((priv->dirty_tx != priv->cur_tx) || netif_queue_stopped(net_dev)) {
            // Get transmit status
            txstatus = ioread32(ioaddr + TSD0 + priv->dirty_tx*sizeof(u32));
             
            // Check if packet hasnt been transmitted yet
            if (!(txstatus & (TxStatOK | TxAborted | TxUnderrun))) 
                break;
                 
            // Update stats for transmitted OK or error
            if (txstatus & TxStatOK) {
                priv->stats.tx_bytes += (txstatus & 0x1fff);
                priv->stats.tx_packets++;
            } else {
                priv->stats.tx_errors++;
            }
             
            // Update dirty buffer
            priv->dirty_tx++;
            priv->dirty_tx = priv->dirty_tx % NUM_TX_DESC;
             
            // Restart network layer if all descritors serviced
            if ((priv->dirty_tx == priv->cur_tx) & netif_queue_stopped(net_dev)) {
                netif_wake_queue(net_dev);
            }
        }
    }
     
    // Receive interrupt: device put a packet in the receive DMA buffer
    if (isr & RxErr) {
        priv->stats.rx_errors++;
    }
    if (isr & RxOK) {
        // while receive ring buffer not empty
        while((ioread8(ioaddr + CR) & RxBufEmpty) == 0) {
            unsigned int rx_status;
            unsigned short rx_size;
            unsigned short pkt_size;
            struct sk_buff *skb;
             
            // wrap ring buffer if at end
            if (priv->cur_rx > RX_BUF_LEN)
                priv->cur_rx = priv->cur_rx % RX_BUF_LEN;
            // first 2 bytes for status, next 2 bytes for frame length
            rx_status = *(u32*)(priv->rx_ring + priv->cur_rx);
                                // priv->rx_ring[priv->cur_rx]
            rx_size = rx_status >> 16;           
            pkt_size = rx_size - 4; // 
             
            // Alloc sk buffer. sk_buff elements:
            //      head: points to start of packet 
            //      data: points to start of packet payload
            //      tail: points to end of packet payload
            //      end:  points to end of packet
            //      len:  amount of data
            // struct sk_buff {         sk_buff_data_t          tail;
            //                          sk_buff_data_t          end;
            //                          unsigned char           *head, *data;
            // sk_buff API:
            //      dev_alloc_skb(len+NET_IP_ALIGN): data, head, tail point to start
            //      skb_reserve(skb, NET_IP_ALIGN): makes space for header. Moves data, tail by NET_IP_ALIGN
            //      memcpy(skb->data, dma_buffer, len): copies payload from DMA to skb after header
            //      skb_put(skb, len): moves tail to end of payload
            //      netif_rx(skb): passes skb to network stack   
            skb = dev_alloc_skb(pkt_size + 2); // extra two bytes for 16byte align
            if (skb) {
                skb->dev = net_dev;
                skb_reserve(skb, 2);
                // copy packet into sk buffe
                memcpy(skb->data, priv->rx_ring + priv->cur_rx + 4, pkt_size);
                // move skb * pointer to
                skb_put(skb, pkt_size);
                 
                skb->protocol = eth_type_trans(skb, net_dev);
                 
                // hand skb to network stack
                netif_rx(skb);
                 
                // NAPI
                 
                // update stats
                net_dev->last_rx = jiffies;
                priv->stats.rx_packets++;
                priv->stats.rx_bytes += pkt_size;
                 
            // Drop packet and update error stats
            } else {
                priv->stats.rx_dropped++;
                if (rx_status & MAR) priv->stats.multicast++;
                if (rx_status & (RUNT|LONG)) priv->stats.rx_length_errors++;
                if (rx_status & CRC) priv->stats.rx_crc_errors++;
                if (rx_status & FAE) priv->stats.rx_frame_errors++;
            }
             
            // update cur_rx to point to next location
            priv->cur_rx = ((priv->cur_rx + rx_size + 4 + 3) & ~3);
             
            // update Current Addr of Packet Register
            iowrite16(priv->cur_rx, ioaddr + CAPR);
        }
    }
     
    // other errors
    if (isr & (CableLen|TimeOut|SysErr))
        printk(KERN_ALERT DRV_NAME ": interrupt for function not implemented\n");
         
out:
 
    return IRQ_HANDLED;
}
 
MODULE_AUTHOR("Jay Medina");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("RTL8139 NIC driver simplified");
 
module_init(my_init);
module_exit(my_exit);

