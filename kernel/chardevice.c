#include <linux/module.h>
#include <linux/init.h>    // for module_init and __init macros
#include <linux/kernel.h>  // for printk
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>  // copy_from/to_user
#include <linux/cdev.h> // class_create
#include <linux/device.h> // class_create
#include <linux/types.h>
#include <linux/kdev_t.h>


#define DRIVER_BUFFER_SIZE 20
#define DEVICE_NAME "chardevice"
#define NUMB_MINORS 10


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("POC for char device interface");
MODULE_AUTHOR("jaymedina@apple.com");

static char device_buffer[DRIVER_BUFFER_SIZE];
static atomic_t currently_opened; 
static DEFINE_SPINLOCK(lock); // used to serialize access to times_opened

static dev_t device_number; // major and minor number in same variable
struct class *class;
struct cdev cdev;
/* macros to get M/m number:
 * MAJOR(device_number) = major number
 * MINOR(device_number) = minor number
 * MKDEV(minor, major) = dev_t vble with both numbers
 */


/* file operations  */
static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
 
static struct file_operations fops = {
    .open = dev_open,
    .release = dev_close,
    .read = dev_read,
    .write = dev_write,
};


static int __init 
dev_init(void) {
 
     int rval;
     printk(KERN_ALERT "Initializing module\n");

    rval = alloc_chrdev_region(&device_number, 0, NUMB_MINORS, DEVICE_NAME);
    if (rval < 0) {
        printk(KERN_ALERT "Device registration failed.\n");
        return rval;
    }
 
    /* populate sysfs entry */
    class = class_create(THIS_MODULE, DEVICE_NAME);
 
    /* initialize cddev and coonnect to fops */
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
 
    /* add char device cdev to system */
    rval = cdev_add(&cdev, device_number, 1);
    if (rval < 0) {
        printk(KERN_ALERT "Device failed to be added to system.\n");
        return rval;
    }
 
    /* create node dynamically and register it with sysfs */
    device_create(class, NULL, device_number, NULL, DEVICE_NAME); 
 
    atomic_set(&currently_opened, 0);
    return 0;
}

static void __exit 
dev_exit(void) {
    printk(KERN_ALERT "Exiting module\n");
    unregister_chrdev_region(device_number, 1);
    device_destroy(class, device_number);
    cdev_del(&cdev);
}


static int 
dev_open(struct inode *inode, struct file *file) {
    static int times_opened = 0;
    spin_lock(&lock);
    times_opened++;
    atomic_add(1, &currently_opened);
    printk(KERN_ALERT "Device number %d opened (%d devices currently opened)\n", 
           times_opened, atomic_read(&currently_opened));
    spin_unlock(&lock);
    return 0;
}
 
static int dev_close(struct inode *inode, struct file *file) {
 
    /* critical section starts (should use lock instead of atomic) */
    atomic_sub(1,&currently_opened); // possible race condition between this and next line
    printk(KERN_ALERT "Device closing (%d devices currently opened)\n",
           atomic_read(&currently_opened)); 
    /* critical section ends */
 
    return 0;
}
 
 
static ssize_t 
dev_read(struct file *file, char __user *user_buffer, size_t count, loff_t *position) {
    
    unsigned long not_copied;
 
    /* offset greater than size of buffer */
    if (*position >= DRIVER_BUFFER_SIZE) {
        return 0;
    }
    /* try to read more than buffer size */
    if (*position + count > DRIVER_BUFFER_SIZE) {
        count = DRIVER_BUFFER_SIZE - *position;
    }
    /* user reading from device */
    not_copied = copy_to_user(user_buffer,
                        device_buffer + *position,
                        count);
    if (not_copied == count) {
        printk(KERN_ALERT "Device couldn't read\n");
        return 0;
    }
 
    *position += count - not_copied;
    return (count - not_copied);
}
 

static ssize_t 
dev_write(struct file *file, const char __user *user_buffer,
          size_t count, loff_t *position) { 
    
    unsigned long not_copied;    
 
    /* try to start writing outside of device buffer */
    if (*position > DRIVER_BUFFER_SIZE) {
        return 0;
    }
    /* try to write more than device buffer size */
    if (*position + count > DRIVER_BUFFER_SIZE) {
        count = DRIVER_BUFFER_SIZE - *position;
    }
     
    /* user writing to device */
    not_copied = copy_from_user(device_buffer,  
                          user_buffer + *position, 
                          count);                 
    if (not_copied == count) {
        printk(KERN_ALERT "Device couldn't write\n");
        return 0;
    }
 
    *position += count - not_copied;
    return (count - not_copied);
}
 
module_init(dev_init);
module_exit(dev_exit);
