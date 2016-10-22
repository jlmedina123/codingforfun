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
 *      usb_free_urb() or usb_unlink_urb()
 *  pipe: address element of each data transfer. encodes endpoint, direction (in, out), exchange type
 *      usb_rcv/snd|ctrl/int/bulk/isoc_pipe()
 *  Descriptors: hold information about device
 *   - usb_device_descriptor
 *   - usb_config_descriptor
 *   - usb_endpoint_descriptor
 */

static struct usb_device_id rtl8150_table[] = {
        {USB_DEVICE(VENDOR_ID_REALTEK, PRODUCT_ID_RTL8150)},
        {}
};

static struct usb_driver rtl8150_driver = {
        .name           = driver_name,
        .probe          = rtl8150_probe,
        .disconnect     = rtl8150_disconnect,
        .id_table       = rtl8150_table,
        .suspend        = rtl8150_suspend,
        .resume         = rtl8150_resume,
        .disable_hub_initiated_lpm = 1,
};

struct rtl8150 {
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
};

static const struct net_device_ops rtl8150_netdev_ops = {
        .ndo_open               = rtl8150_open,
        .ndo_stop               = rtl8150_close,
        .ndo_do_ioctl           = rtl8150_ioctl,
        .ndo_start_xmit         = rtl8150_start_xmit,
        .ndo_tx_timeout         = rtl8150_tx_timeout,
        .ndo_set_rx_mode        = rtl8150_set_multicast,
        .ndo_set_mac_address    = rtl8150_set_mac_address,

        .ndo_change_mtu         = eth_change_mtu,
        .ndo_validate_addr      = eth_validate_addr,
};

static int rtl8150_probe(struct usb_interface *intf,
                         const struct usb_device_id *id)
{
        struct usb_device *udev = interface_to_usbdev(intf);
        rtl8150_t *dev;
        struct net_device *netdev;

        netdev = alloc_etherdev(sizeof(rtl8150_t));
        if (!netdev)
                return -ENOMEM;

        dev = netdev_priv(netdev);

        dev->intr_buff = kmalloc(INTBUFSIZE, GFP_KERNEL);
        if (!dev->intr_buff) {
                free_netdev(netdev);
                return -ENOMEM;
        }

        tasklet_init(&dev->tl, rx_fixup, (unsigned long)dev);
        spin_lock_init(&dev->rx_pool_lock);


        dev->udev = udev;
        dev->netdev = netdev;
        netdev->netdev_ops = &rtl8150_netdev_ops;
        netdev->watchdog_timeo = RTL8150_TX_TIMEOUT;
        SET_ETHTOOL_OPS(netdev, &ops);
        dev->intr_interval = 100;       /* 100ms */

        if (!alloc_all_urbs(dev)) {
                dev_err(&intf->dev, "out of memory\n");
                goto out;
        }
        if (!rtl8150_reset(dev)) {
                dev_err(&intf->dev, "couldn't reset the device\n");
                goto out1;
        }
        fill_skb_pool(dev);
        set_ethernet_addr(dev);

        usb_set_intfdata(intf, dev);
        SET_NETDEV_DEV(netdev, &intf->dev);
        if (register_netdev(netdev) != 0) {
                dev_err(&intf->dev, "couldn't register the device\n");
                goto out2;
        }
        dev_info(&intf->dev, "%s: rtl8150 is detected\n", netdev->name);

        return 0;

out2:
        usb_set_intfdata(intf, NULL);
        free_skb_pool(dev);
out1:
        free_all_urbs(dev);
out:
        kfree(dev->intr_buff);
        free_netdev(netdev);
        return -EIO;
}



static int alloc_all_urbs(rtl8150_t * dev)
{
        dev->rx_urb = usb_alloc_urb(0, GFP_KERNEL);
        if (!dev->rx_urb)
                return 0;
        dev->tx_urb = usb_alloc_urb(0, GFP_KERNEL);
        if (!dev->tx_urb) {
                usb_free_urb(dev->rx_urb);
                return 0;
        }
        dev->intr_urb = usb_alloc_urb(0, GFP_KERNEL);
        if (!dev->intr_urb) {
                usb_free_urb(dev->rx_urb);
                usb_free_urb(dev->tx_urb);
                return 0;
        }

        return 1;
}

static void fill_skb_pool(rtl8150_t *dev)
{
        struct sk_buff *skb;
        int i;

        for (i = 0; i < RX_SKB_POOL_SIZE; i++) {
                if (dev->rx_skb_pool[i])
                        continue;
                skb = dev_alloc_skb(RTL8150_MTU + 2);
                if (!skb) {
                        return;
                }
                skb_reserve(skb, 2);
                dev->rx_skb_pool[i] = skb;
        }
}

static int get_registers(rtl8150_t * dev, u16 indx, u16 size, void *data)
{
        return usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
                               RTL8150_REQ_GET_REGS, RTL8150_REQT_READ,
                               indx, 0, data, size, 500);
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

