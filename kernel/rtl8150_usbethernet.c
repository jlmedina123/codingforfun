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
 *
 *
