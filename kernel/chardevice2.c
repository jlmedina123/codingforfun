/*
 * TODO: add blocking and polling
 */


/* character device
 *
 * Makefile:
 *
 * obj-m += chardriver.o
 *  
 *  all:
 *     make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 *           
 *  clean:
 *     make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
 *
 * 

jose@jose:~/Documents/practive$ ls
chardriver.c  chardriver.c~  Makefile
jose@jose:~/Documents/practive$ make
make -C /lib/modules/3.13.0-30-generic/build M=/home/jose/Documents/practive modules
make[1]: Entering directory `/usr/src/linux-headers-3.13.0-30-generic'
  CC [M]  /home/jose/Documents/practive/chardriver.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /home/jose/Documents/practive/chardriver.mod.o
  LD [M]  /home/jose/Documents/practive/chardriver.ko
make[1]: Leaving directory `/usr/src/linux-headers-3.13.0-30-generic'
jose@jose:~/Documents/practive$ 
jose@jose:~/Documents/practive$ ls -latr /dev/charmoduleexample 
crw------- 1 root root 249, 0 Aug 28 23:15 /dev/charmoduleexample
jose@jose:~/Documents/practive$ sudo chmod 777 /dev/charmoduleexample 
jose@jose:~/Documents/practive$ echo "test1" > /dev/charmoduleexample 
jose@jose:~/Documents/practive$ cat /dev/charmoduleexample 
test1
jose@jose:~/Documents/practive$ sudo rmmod chardriver 

*/

#include <linux/module.h>
#include <linux/init.h>    // for module_init and __init macros
#include <linux/kernel.h>  // for printk
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>   // copy_from/to_user
#include <linux/cdev.h>    // class_create
#include <linux/device.h>  // class_create
#include <linux/types.h>
#include <linux/kdev_t.h>
 
//#define OLDWAY_REGIST_CHARDEVICE
#define DRIVER_BUFFER_SIZE 20
#define DEVICE_NAME "charmoduleexample"
#define NUMB_MINORS 10
 
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Char driver");
MODULE_AUTHOR("J Luis Medina");
 
 
/* global static vble means it's only accessible within
   the file they are declared in
 */
static char device_buffer[DRIVER_BUFFER_SIZE];
static atomic_t currently_opened; // should use lock instead, but trying atomic type
static DEFINE_SPINLOCK(lock); // used to serialize access to times_opened
 
#ifndef OLDWAY_REGIST_CHARDEVICE
static dev_t device_number; // major and minor number in same variable
struct class *class;
struct cdev cdev;
/* macros to get M/m number:
      MAJOR(device_number) = major number
      MINOR(device_number) = minor number
      MKDEV(minor, major) = dev_t vble with both numbers
*/
#else
static unsigned int major;
#endif
 
 
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
 
 
/* __init macro causes the init function to be discarded and its memory freed 
   once the init function finishes for built-in drivers, but not loadable modules
   __exit macro causes the omission of the function when the module is built 
   into the kernel. It has no effect for loadable modules
 */
static int __init dev_init(void) {
 
     int rval;
     printk(KERN_ALERT "Initializing module\n");
 
#ifdef OLDWAY_REGIST_CHARDEVICE
     // register major number for character device
     major = register_chrdev(0, // (unsigned int) major number. 0 for dynamic allocation 
                             DEVICE_NAME, // (cont char *) device name
                             &fops);  // (const struct file_operation *) fops for device
     if (major < 0) {
         printk(KERN_ALERT "Device registration failed.\n");
         return major;
     } else {
         printk(KERN_ALERT "Device allocated with major %u\n", major);
     }
 
#else
     
    rval = alloc_chrdev_region(&device_number, // (dev_t *) output parameter for first assigned number
                               0,     // (unsigned) first of the requested range of minor numbers
                               NUMB_MINORS, // (unsigned) the number of minor numbers required 
                               DEVICE_NAME); // (const char *) the name of the associated driver 
    if (rval < 0) {
        printk(KERN_ALERT "Device registration failed.\n");
        return rval;
    }
 
    /* populate sysfs entry */
    class = class_create(THIS_MODULE, DEVICE_NAME);
 
    /* initialize cddev and connect to fops */
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
 
    /* add char device cdev to system */
    rval = cdev_add(&cdev, device_number, 1);
    if (rval < 0) {
        printk(KERN_ALERT "Device failed to be added to system.\n");
        return rval;
    }
 
    /* create node dynamically and register it with sysfs */
     
    device_create(class, // (struct class *) class device is registered to
                  NULL,  // (struct device * parent) parrent device
                  device_number, // (dev_t devt) device number
                  NULL,  // (void *drvdata) data to be added to device for callbacks
                  DEVICE_NAME); //(const char *) device name
     
#endif
 
     atomic_set(&currently_opened, 0);
     return 0;
}
 
static void __exit dev_exit(void) {
 
     printk(KERN_ALERT "Exiting module\n");
#ifdef OLDWAY_REGIST_CHARDEVICE 
    unregister_chrdev(major, DEVICE_NAME);
#else
    unregister_chrdev_region(device_number, 1);
    device_destroy(class, device_number);
    cdev_del(&cdev);
#endif
}
 
 
static int dev_open(struct inode *inode,
                    struct file *file) {
    /* local static variable means that when function called, it will
       get the last value from last time function called. The first
       time it will get the value defined
    */
    static int times_opened = 0;
    /* critical section starts */
    spin_lock(&lock);
    times_opened++;
    atomic_add(1, &currently_opened);
    printk(KERN_ALERT "Device number %d opened (%d devices currently opened)\n", 
           times_opened, atomic_read(&currently_opened));
    spin_unlock(&lock);
    /* critical section finishes */
     
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
 
static ssize_t dev_read(struct file *file,
                       char __user *user_buffer,
                       size_t count,
                       loff_t *position) { // file position user is accessing
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
 
static ssize_t dev_write(struct file *file,
                        const char __user *user_buffer,
                        size_t count,         // number of bytes requested to write
                        loff_t *position) {   // file (device) position user accessing
                                              // returns number of bytes copied
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
    not_copied = copy_from_user(device_buffer,  // (void *) destination in kernel 
                          user_buffer + *position, // (const void __user *) source in user space
                          count);                  // (unsigned long): number of bytes to copy
                                                   // return (unsigned long): number of bytes not copied
    if (not_copied == count) {
        printk(KERN_ALERT "Device couldn't write\n");
        return 0;
    }
 
    *position += count - not_copied;
    return (count - not_copied);
}
 
module_init(dev_init);
module_exit(dev_exit);

