/*
 * based off drivers/net/usb/rtl8150.c
 */

/* Types of data exchange:
 *  - control transfer
 *  - bulk transfer: large data, time-insensitive
 *  - interrupt transfer: small data, time-sensitive
 *  - isonchronous transfer: real-time data
 *  Endpoint: addressable unit. unidirectional (IN upstream or OUT downstream)
 *  Device memory not mapped to CPU memory (unlike PCI)
 *  struct usb_device
 *  struct urb: API
 *      1) usb_alloc_urb()
 *      2) usb_fill_control/int/bulk_urb()
 *      3) usb_submit_urb()
 *      or usb_control_interrupt_bulk_msg()
 *      4) usb_free_urb() or usb_unlink_urb()
 *  pipe: address element of each data transfer. encodes endpoint, direction (in, out), exchange type
 *      usb_rcv/snd|ctrl/int/bulk/isoc_pipe()
 *  Descriptors: hold information about device
 *   - usb_device_descriptor
 *   - usb_config_descriptor
 *   - usb_interface_descriptor
 *   - usb_endpoint_descriptor
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

	// 3) Allocate interrupt buffer
	dev->intr_buff = kmalloc(INTBUFSIZE, GFP_KERNEL);

	// 4) Allocate URBs
	dev->rx_urb = usb_alloc_urb(0, GFP_KERNEL);
        dev->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
        dev->intr_urb = usb_alloc_urb(0, GFP_KERNEL);
        
	// 5) Allocate sk buffers
        for (i = 0; i < RX_SKB_POOL_SIZE; i++) {
            	skb = dev_alloc_skb(RTL8150_MTU + 2);
                skb_reserve(skb, 2);
                dev->rx_skb_pool[i] = skb;
        }

	// 6) Get Ethernet address and set it up
        u8 node_id[6];
        get_registers(dev, IDR, sizeof(node_id), node_id);
        memcpy(dev->netdev->dev_addr, node_id, sizeof(node_id));

	// 7) Attach private data to interface and to net device 
        usb_set_intfdata(intf, dev);
        SET_NETDEV_DEV(netdev, &intf->dev);

	// 8) Register net device
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
        return usb_control_msg(dev->udev, 	     /* struct usb_device */
			       pipe,	             /* endpoint pipe to send msg to */
                               RTL8150_REQ_GET_REGS, /* USB msg request */ 
			       RTL8150_REQT_READ,    /* USB msg request type */
                               indx,		     /* USB msg value */ 
				0,		     /* USB msg index value */ 
				data,		     /* data */ 
				size,		     /* lenghts of data in bytes */ 
				500);		     /* time to wait in msecs before timing out */
}

static int set_registers(rtl8150_t * dev, u16 indx, u16 size, void *data)
{
        return usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
                               RTL8150_REQ_SET_REGS, RTL8150_REQT_WRITE,
                               indx, 0, data, size, 500);
}



/*
 * Net device operations
 */

static int rtl8150_open(struct net_device *netdev)
{
        rtl8150_t *dev = netdev_priv(netdev);
        int res;

        if (dev->rx_skb == NULL)
                dev->rx_skb = pull_skb(dev);

        set_registers(dev, IDR, 6, netdev->dev_addr);

        usb_fill_bulk_urb(dev->rx_urb, dev->udev, usb_rcvbulkpipe(dev->udev, 1),
                      dev->rx_skb->data, RTL8150_MTU, read_bulk_callback, dev);
        usb_submit_urb(dev->rx_urb, GFP_KERNEL);

        usb_fill_int_urb(dev->intr_urb, dev->udev, usb_rcvintpipe(dev->udev, 3),
                     dev->intr_buff, INTBUFSIZE, intr_callback,
                     dev, dev->intr_interval);
        usb_submit_urb(dev->intr_urb, GFP_KERNEL);

        usb_fill_int_urb(dev->intr_urb, dev->udev, usb_rcvintpipe(dev->udev, 3),
                     dev->intr_buff, INTBUFSIZE, intr_callback,
                     dev, dev->intr_interval);
        usb_submit_urb(dev->intr_urb, GFP_KERNEL);
        

        enable_net_traffic(dev);
        set_carrier(netdev);

        netif_start_queue(netdev);

        return res;
}

