/*
 * based off drivers/net/usb/rtl8150.c
 */

/* USB protocol: support hotplugging, generic class drivers, various types of data transfer,
 * 4-wire serial point-to-point connection.
 *
 * USB subsystem:
 *  - USB core: to help write client drivers independend of host controllers
 *  - USB host controller driver
 *  - khubd: helper kernel thread that monitors ports connected to USB hub
 *  - usbfs: filesystem to control USB devices from user space
 *
 * USB types of packets: 
 *  - Token packet: indicates type of transaction
 *  - Data packet
 *  - Handshake packet: for ack data or error
 *  - start of frame packet
 *
 * Enumeration: when device is connected to host, host sends control transaction to read 
 * device descriptors. Host then decices type of device driver
 *
 * Two types of USB drivers:
 *  - USB device driver: runs on a host system and controls the USB device connected to it
 *  - USB gadget driver: runs on a device and controls how the device looks to the host computer
 *
 * Types of data exchange (endpoints):
 *  - control transfer
 *  - bulk transfer: large data, time-insensitive
 *  - interrupt transfer: small data, time-sensitive
 *  - isonchronous transfer: real-time data
 *
 *  USB device components:
 *   - Endpoint (struct usb_host_endpoint): addressable unit. carries data in one 
 *   direction (IN upstream or OUT downstream). Endpoint 0 exclusively for device configuration
 *   - Interface (struct usb_interface): bundles multiple endpoints within one single logical 
 *   connection (mouse, keyboard, etc)
 *   - Configuration: bundle of interfaces. One USB device can have multiple configurations, and switch them
 * to change states
 *     Defined by struct usb_host_config
 *   - Device. defined by struct usb_device
 *
 *  Device memory not mapped to CPU memory (unlike PCI)
 *  
 * URB: used for all communication between host and device, to send/receive data 
 * to/from end point. Defined by struct urb. Every urb has a completion handler
 *
 * struct urb:
 *     struct usb_device *dev
 *      unsigned int pipe: endpoint the urb is sent to. API to set it up:
 *        uint usb_sndctrlpipe(dev, endpoint): specifies control OUT endpoint
 *        uint usb_rcvctrlpipe(dev, endpoint): specifies control IN endpoint
 *        uint usb_sndbulkpipe(dev, endpoint): specifies bulk OUT endpoint
 *        uint usb_rcvbulkpipe(dev, endpoint): 
 *        uint usb_sndintpipe(dev, endpoint):
 *        uint usb_rcvintpipe(dev, endpoint):
 *        uint usb_sndisocpipe
 *        uint usbrcvisopipe
 *    unsigned int transfer_flags
 *    void *buffer
 *    dmd_addr_t transfer_dma
 *    ...
 *  API:
 *      1) usb_alloc_urb()
 *      2) usb_fill_control/int/bulk_urb()
 *      3) usb_submit_urb(): returns 0 for request queued, or <0 for error
 *      or usb_control_interrupt_bulk_msg()
 *      4) usb_free_urb() 
 *      To cancel urb: usb_kill_urb() or usb_unlink_urb()
 * 
 * Completion handler:
 * usb_submit_urb: 0 returned if call successful, completion handler called once.
 * Runs in interrupt context
 * completion handler called in three situations:
 *    1) urb successufully sent, and ack recevied. urb->status == 0
 *    2) error happened during transfered: urb->status != 0
 *    3) urb unlinked. urb canceled by user, or usb device unplugged    
 *
 * Descriptors: hold information about device
 *   - usb_device_descriptor
 *   - usb_config_descriptor
 *   - usb_interface_descriptor
 *   - usb_endpoint_descriptor
 *
 * USB classes: if a device is plugged into the USB hub and belongs to a known
 * usb class, a standard USB driver can be used. The device interface descriptor
 * needs to conform to the standard of the USB class.
 *   - 0x08: mass storage
 *   - ...
 * 
 *
 */

 /* RTL8150 device specs:
  *   Endpoints
  *      - 0 device configuration (as standard)
  *      - 1 BULK IN: to transfer packets to host. Incoming Ethernet packet queded in device's
  *        rx buffer. Packets transfered to host memory
  *      - 2 BULK OUT
  *      - 3 interrupt IN: to poll device stats (such as transmit errors)  
  */

/*
 * USB driver structs
 */
static struct usb_device_id rtl8150_table[] = {
        {USB_DEVICE(VENDOR_ID_REALTEK, PRODUCT_ID_RTL8150)},
        {}
};

static struct usb_driver rtl8150_driver = {
        .name           = driver_name,
        .probe          = rtl8150_probe,
        //.disconnect     = rtl8150_disconnect,
        .id_table       = rtl8150_table,
        //.suspend        = rtl8150_suspend,
        //.resume         = rtl8150_resume,
        .disable_hub_initiated_lpm = 1,
};

/* 
 * Driver private data
 */
typedef struct rtl8150 {
        unsigned long flags;
        struct usb_device *udev;
        struct tasklet_struct tl;
        struct net_device *netdev;
        struct urb *rx_urb, *tx_urb, *intr_urb;
        struct sk_buff *tx_skb, *rx_skb;
        struct sk_buff *rx_skb_pool[RX_SKB_POOL_SIZE];
        spinlock_t rx_pool_lock;
        struct usb_ctrlrequest dr;
        int intr_interval;
        u8 *intr_buff;
        u8 phy;
} rtl8150_t;

/*
 * Net device operations
 */
static const struct net_device_ops rtl8150_netdev_ops = {
        .ndo_open               = rtl8150_open,
        //.ndo_stop               = rtl8150_close,
        .ndo_do_ioctl           = rtl8150_ioctl,
        .ndo_start_xmit         = rtl8150_start_xmit,
        //.ndo_tx_timeout         = rtl8150_tx_timeout,
        //.ndo_set_rx_mode        = rtl8150_set_multicast,
        //.ndo_set_mac_address    = rtl8150_set_mac_address,
        //.ndo_change_mtu         = eth_change_mtu,
        //.ndo_validate_addr      = eth_validate_addr,
};


static int rtl8150_probe(struct usb_interface *intf, const struct usb_device_id *id);
static int rtl8150_open(struct net_device *netdev);
static netdev_tx_t rtl8150_start_xmit(struct sk_buff *skb, struct net_device *netdev);
static void read_bulk_callback(struct urb *urb);
static void write_bulk_callback(struct urb *urb);
static void intr_callback(struct urb *urb);
static int get_registers(rtl8150_t * dev, u16 indx, u16 size, void *data);
static int set_registers(rtl8150_t * dev, u16 indx, u16 size, void *data);




/*
 * module methods
 */
module_usb_driver(rtl8150_driver);



/* macros for module init/exit: 
    
#define module_usb_driver(__usb_driver) \
        module_driver(__usb_driver, usb_register, \
                       usb_deregister)
#define usb_register(driver) \
        usb_register_driver(driver, THIS_MODULE, KBUILD_MODNAME)

#define module_driver(__driver, __register, __unregister, ...) \
static int __init __driver##_init(void) \
{ \
        return __register(&(__driver) , ##__VA_ARGS__); \
} \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
        __unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);

*/



/*
 * USB driver functions 
 */

static int rtl8150_probe(struct usb_interface *intf,
                         const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    rtl8150_t *dev;
    struct net_device *netdev;
    struct sk_buff *skb;
    int i;

    // 1) Alloc Ethernet device and initialize it
    netdev = alloc_etherdev(sizeof(rtl8150_t));
    netdev->netdev_ops = &rtl8150_netdev_ops;
    netdev->watchdog_timeo = RTL8150_TX_TIMEOUT;
    SET_ETHTOOL_OPS(netdev, &ops);


    // 2) Get private data from netdev and initialize it
    dev = netdev_priv(netdev);

    dev->intr_buff = kmalloc(INTBUFSIZE, GFP_KERNEL);
    tasklet_init(&dev->tl, rx_fixup, (unsigned long)dev);
    spin_lock_init(&dev->rx_pool_lock);
    dev->udev = udev;
    dev->netdev = netdev;
    dev->intr_interval = 100;       /* 100ms */
    tasklet_init(&dev->tl, rx_fixup, (unsigned long)dev);

    // 2.1) Allocate interrupt buffer
    dev->intr_buff = kmalloc(INTBUFSIZE, GFP_KERNEL);

    // 2.2) Allocate URBs
    dev->rx_urb = usb_alloc_urb(0, GFP_KERNEL);
    dev->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
    dev->intr_urb = usb_alloc_urb(0, GFP_KERNEL);
        
    // 2.3) Allocate sk buffers
    for (i = 0; i < RX_SKB_POOL_SIZE; i++) {
        skb = dev_alloc_skb(RTL8150_MTU + 2);
        skb_reserve(skb, 2);
        dev->rx_skb_pool[i] = skb;
    }

    // 2.4) Get Ethernet address and set it up
    u8 node_id[6];
    get_registers(dev, IDR, sizeof(node_id), node_id);
    memcpy(dev->netdev->dev_addr, node_id, sizeof(node_id));

    // 3) Attach private data to interface 
    usb_set_intfdata(intf, dev);
    // 4) Set sysfs device reference
    SET_NETDEV_DEV(netdev, &intf->dev);

    // 5) Register net device
    register_netdev(netdev); 

    return 0;
}


static int get_registers(rtl8150_t * dev, u16 indx, u16 size, void *data)
{
    /* specify control IN endpoint number 0 
       usb_rcvctrlpipe macro: (dev->devnum << 8) | (endpoint << 15)
    */
    unsigned int pipe = usb_rcvctrlpipe(dev->udev, 0);
    /* build control URB, send it off, and wait for completion
       usb_control_msg()
        struct usb_ctrlrequest *dr = kmalloc(sizeof(struct usb_ctrlrequest), GFP_NOIO);
            dr->bRequestType = requesttype;  
                dr->bRequest = request;
                dr->wValue = cpu_to_le16(value);
                dr->wIndex = cpu_to_le16(index);
                dr->wLength = cpu_to_le16(size);
                ret = usb_internal_control_msg(dev, pipe, dr, data, size, timeout);
                kfree(dr);
    */
    return usb_control_msg(dev->udev,          /* struct usb_device */
                   pipe,                 /* endpoint pipe to send msg to */
                   RTL8150_REQ_GET_REGS, /* USB msg request */ 
                   RTL8150_REQT_READ,    /* USB msg request type */
                   indx,             /* USB msg value */ 
                   0,             /* USB msg index value */ 
                   data,             /* data */ 
                   size,             /* lenghts of data in bytes */ 
                   500);             /* time to wait in msecs before timing out */
}

static int set_registers(rtl8150_t * dev, u16 indx, u16 size, void *data)
{
        // Builds a control urb, sends it off and waits for completion
        return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
                               RTL8150_REQ_SET_REGS, RTL8150_REQT_WRITE,
                               indx, 0, data, size, 500);
}



/*
 * Net device operations
 */

/* rtl8150_open fills, submits the bulk in and interrupt in URBs 
 * and allows transmission (netif_start_queue)*/
static int rtl8150_open(struct net_device *netdev)
{
        rtl8150_t *dev = netdev_priv(netdev);
        int res;

        // get skb from rx_skb_pool
        if (dev->rx_skb == NULL)
                dev->rx_skb = pull_skb(dev); 

        // set Ethernet address to Input Data Register
        set_registers(dev, IDR, 6, netdev->dev_addr);

        // 1) Set up pipe for builk IN (endpoint 1) for receiving packets from device
        // 1.1) Allocate urb (done in probe)
        // 1.2) populate bulk urb and register callback
        usb_fill_bulk_urb(dev->rx_urb, // struct urb * urb
            dev->udev,                 // struct usb_device * dev,
            usb_rcvbulkpipe(dev->udev, 1), // unsigned int pipe,
            dev->rx_skb->data,         // void * transfer_buffer,
            RTL8150_MTU,               // int buffer_length, 
            read_bulk_callback,        // usb_complete_t complete_fn,
            dev);                      // void * context);
        // 1.3) submit bulk urb
        usb_submit_urb(dev->rx_urb, GFP_KERNEL);

        // 2) Set up pipe for interrupt IN (endpoint 3) for receiving errors and status
        // 2.1) Allocate urb (done in probe)
        // 2.2) populate int urb and register callback
        usb_fill_int_urb(dev->intr_urb, 
            dev->udev, 
            usb_rcvintpipe(dev->udev, 3),
            dev->intr_buff, 
            INTBUFSIZE, 
            intr_callback,
            dev, 
            dev->intr_interval);
        // 2.3) submit int urb
        usb_submit_urb(dev->intr_urb, GFP_KERNEL);

        // 3) reset device and configure it
        enable_net_traffic(dev);
        
        // netif_carrier_on(netdev) if link_status on
        set_carrier(netdev);

        // allow transmission
        netif_start_queue(netdev);

        return res;
}
/*
enum netdev_tx {
        __NETDEV_TX_MIN  = INT_MIN,     // make sure enum is signed 
        NETDEV_TX_OK     = 0x00,        // driver took care of packet 
        NETDEV_TX_BUSY   = 0x10,        // driver tx path was busy
        NETDEV_TX_LOCKED = 0x20,        // driver tx lock was already taken 
};
typedef enum netdev_tx netdev_tx_t;
*/
static netdev_tx_t rtl8150_start_xmit(struct sk_buff *skb,
                                      struct net_device *netdev)
{
        rtl8150_t *dev = netdev_priv(netdev);
        int count, res;

        netif_stop_queue(netdev);
        count = (skb->len < 60) ? 60 : skb->len;
        count = (count & 0x3f) ? count : count + 1;
        dev->tx_skb = skb;
        usb_fill_bulk_urb(dev->tx_urb, dev->udev, usb_sndbulkpipe(dev->udev, 2),
                      skb->data, count, write_bulk_callback, dev);
        if ((res = usb_submit_urb(dev->tx_urb, GFP_ATOMIC))) {
                /* Can we get/handle EPIPE here? */
                if (res == -ENODEV)
                        netif_device_detach(dev->netdev);
                else {
                        dev_warn(&netdev->dev, "failed tx_urb %d\n", res);
                        netdev->stats.tx_errors++;
                        netif_start_queue(netdev);
                }
        } else {
                netdev->stats.tx_packets++;
                netdev->stats.tx_bytes += skb->len;
                netdev->trans_start = jiffies;
        }

        return NETDEV_TX_OK;
}

/*
 * enum netdev_tx {
 *    __NETDEV_TX_MIN  = INT_MIN, // make sure enum is signed
 *    NETDEV_TX_OK     = 0x00,    // driver took care of packet 
 *    NETDEV_TX_BUSY   = 0x10,    // driver tx path was busy
 *    NETDEV_TX_LOCKED = 0x20,    // driver tx lock was already taken 
 * };
 * typedef enum netdev_tx netdev_tx_t;
 */

/* rtl8150_start_xmit called by protocol layer to transfer
 * skb to NIC driver
 */
static netdev_tx_t rtl8150_start_xmit(struct sk_buff *skb,
                        struct net_device *netdev)
{
    rtl8150_t *dev = netdev_priv(netdev);
    int count, res;

    // 1) stop until transmission completed
    netif_stop_queue(netdev);
    
    count = (skb->len < 60) ? 60 : skb->len;
    count = (count & 0x3f) ? count : count + 1;
    
    // 2) point tx_skb to skb received from protocol engine
    dev->tx_skb = skb;
    
    // 3) allocate bulk URB (done in probe)
 
    // 4) Populate bulk URB and register callback
    usb_fill_bulk_urb(dev->tx_urb, dev->udev, usb_sndbulkpipe(dev->udev, 2),
              skb->data, count, write_bulk_callback, dev);
    // 5) Submit bulk URB with skb
    if ((res = usb_submit_urb(dev->tx_urb, GFP_ATOMIC))) {
        // if operation not supported by device, detach device
		if (res == -ENODEV)
            netif_device_detach(dev->netdev);
		// update stats with result
        else {
            dev_warn(&netdev->dev, "failed tx_urb %d\n", res);
            netdev->stats.tx_errors++;
            netif_start_queue(netdev);
        }
    } else {
        netdev->stats.tx_packets++;
        netdev->stats.tx_bytes += skb->len;
        netdev->trans_start = jiffies;
    }

    return NETDEV_TX_OK;
}


/*
 * Callback functions
 */

/* Asynchronous bulk submit requires registering a callback function to usb_fill_bulk_urb()
 * Function called when packet arrives, skb copied into urb
 */ 
static void read_bulk_callback(struct urb *urb)
{
    rtl8150_t *dev;
    unsigned pkt_len, res;
    struct sk_buff *skb;
    struct net_device *netdev;
    u16 rx_stat;
    int status = urb->status;
    int result;

    // 1) Check status for errors
    switch (status) {
    case 0:
        break;
    case -ENOENT:  // No such file or directory 
        return; /* the urb is in unlink state */
    case -ETIME: // timeout
        dev_warn(&urb->dev->dev, "may be reset is needed?..\n");
        goto reschedule;
    default:
        dev_warn(&urb->dev->dev, "Rx status %d\n", status);
        goto reschedule;
    }
   
    // 2) Get packet size
    res = urb->actual_length; // size of packet received
    pkt_len = res - 4; //first 4 bytes are rx header and CRC

	// 3) update skb->len and skb->tail to point to payload
    skb_put(dev->rx_skb, pkt_len); 

	// 4) set skb protocol in header
    dev->rx_skb->protocol = eth_type_trans(dev->rx_skb, netdev);

	// 5) hand over skb to protocol layer
    netif_rx(dev->rx_skb); 

    // 6) Update stats
    netdev->stats.rx_packets++;
    netdev->stats.rx_bytes += pkt_len;

    // 7) Prepare for next packet recieve from device
    // 7.1) Allocate skb or get skb from pool
    spin_lock(&dev->rx_pool_lock);
    skb = pull_skb(dev);
    spin_unlock(&dev->rx_pool_lock);
    if (!skb)
        goto resched;

    // 7.2) Point rx_skb to allocated skb
    dev->rx_skb = skb;


reschedule:

    // 7.3) Fill rcv bulk URB and register callback
    usb_fill_bulk_urb(dev->rx_urb, dev->udev, usb_rcvbulkpipe(dev->udev, 1),
              dev->rx_skb->data, RTL8150_MTU, read_bulk_callback, dev);
    // 7.4) Submit (interrupt context!!!)
    result = usb_submit_urb(dev->rx_urb, GFP_ATOMIC);
    // 7.5) Check for errors
    if (result == -ENODEV)
        netif_device_detach(dev->netdev);
    else if (result) {
        set_bit(RX_URB_FAIL, &dev->flags);
        goto resched;
    } else {
        clear_bit(RX_URB_FAIL, &dev->flags);
    }

    return;

resched:
    // if submit URB failed, fix it in tasklet
    tasklet_schedule(&dev->tl);
}

/* write_bulk_callback frees skb and restarts queue
 */
static void write_bulk_callback(struct urb *urb)
{
    rtl8150_t *dev;
    int status = urb->status;

    dev = urb->context;
    if (!dev)
        return;
    // free skb from interrupt context
    dev_kfree_skb_irq(dev->tx_skb); 

    if (!netif_device_present(dev->netdev))
        return;
    if (status)
        dev_info(&urb->dev->dev, "%s: Tx status %d\n",
             dev->netdev->name, status);
    dev->netdev->trans_start = jiffies;
    netif_wake_queue(dev->netdev);
}

/* intr_callback used for error notifications. The device sends tx
 * errors and link status chagnes via interrupt urb endpoint 3
 * callback registered with usb_fill_intr_urb()
 */
static void intr_callback(struct urb *urb)
{
    rtl8150_t *dev;
    __u8 *d;
    int status = urb->status;
    int res;

    // 1) check URB status
    switch (status) {
    case 0:         /* success */
        break;
    case -ECONNRESET: // Connection reset by peer (unlink)
    case -ENOENT:     // No such file or directory
    case -ESHUTDOWN:  // Cannot send after transport endpoint shutdown
        return;
    /* -EPIPE:  should clear the halt */
    default:
        dev_info(&urb->dev->dev, "%s: intr status %d\n",
             dev->netdev->name, status);
        goto resubmit;
    }

    // 2) Check for tx errors 
    d = urb->transfer_buffer;
    if (d[0] & TSR_ERRORS) {
        dev->netdev->stats.tx_errors++;
        if (d[INT_TSR] & (TSR_ECOL | TSR_JBR))
            dev->netdev->stats.tx_aborted_errors++;
        if (d[INT_TSR] & TSR_LCOL)
            dev->netdev->stats.tx_window_errors++;
        if (d[INT_TSR] & TSR_LOSS_CRS)
            dev->netdev->stats.tx_carrier_errors++;
    }
    // 3) Check for link errors, and report link changes to the network stack 
    if ((d[INT_MSR] & MSR_LINK) == 0) {
        if (netif_carrier_ok(dev->netdev)) {
            netif_carrier_off(dev->netdev);
            netdev_dbg(dev->netdev, "%s: LINK LOST\n", __func__);
        }
    } else {
        if (!netif_carrier_ok(dev->netdev)) {
            netif_carrier_on(dev->netdev);
            netdev_dbg(dev->netdev, "%s: LINK CAME BACK\n", __func__);
        }
    }

resubmit:

    // 4) resubmit urb
    res = usb_submit_urb (urb, GFP_ATOMIC);
    if (res == -ENODEV)
        netif_device_detach(dev->netdev);
    else if (res)
        dev_err(&dev->udev->dev,
            "can't resubmit intr, %s-%s/input0, status %d\n",
            dev->udev->bus->bus_name, dev->udev->devpath, res);
}

