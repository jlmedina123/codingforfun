#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>         // struct file_operations
#include <linux/sched.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>      // put_user
#include <linux/tty.h>        // tty
#include <linux/wait.h>       // wait_event_interruptible
#include <linux/poll.h>
#include <linux/device.h>     // dynamic device node creation     
 
static char my_devname[]= "poll_dev"; // appears in /proc/devices
                                      // used also for device node creation
 
#define  N 23
static char rbuf[N+1];  
static int  use_count;
static int  ir, iw, ih;
static struct class *poll_class; // for dynamic device node creation
 
static spinlock_t lock = SPIN_LOCK_UNLOCKED; // to serialize access to common resources
 
#define DEV_MAJOR   249 // for static  major and minor device node 
#define DEV_MINOR   5
 
// wait queues wake up all processes waiting on them
// so wait queue for readers and writers need to be separate
static DECLARE_WAIT_QUEUE_HEAD(qin);  // Wait queue head for read event
static DECLARE_WAIT_QUEUE_HEAD(qout); // Wait queue head for write event 
 
static struct semaphore sema;
 
 
 
static int is_buffer_full(void)
{
    ih = ir-1;
    if (ih < 0)
      ih += N;
 
    if (iw == ih) 
         return 1;
 
     return 0;
 
}
static int is_buffer_empty(void)
{
      if (ir == iw)
         return 1;
 
     return 0;
}
 
static unsigned int device_poll(struct file *filp, poll_table *wait)
{
        unsigned int mask=0;
 
        poll_wait(filp, &qin, wait);
 
        /* Serialize access to buffer */
        spin_lock(&lock);
 
        // READ
        if(!is_buffer_empty()) // buffer not empty. read wont block 
        {
          printk ("%s - POLLIN EVENT:ir=%d|iw=%d\n", my_devname,ir,iw);
          mask |= POLLIN | POLLRDNORM;  /* fd is readable */
        }
 
        // WRITE
        if(!is_buffer_full()) // buffer not full. write wont block
        {
          printk ("%s - POLLOUT EVENT:ir=%d|iw=%d\n", my_devname,ir,iw);
          mask |= POLLOUT | POLLWRNORM;  /* fd is writable */
        }
 
    spin_unlock(&lock);
 
        return mask;
}
 
 
static ssize_t device_read(struct file *filp, 
                           char *buffer, size_t len, loff_t *offs)
{
    // possible race conditions???
    if (is_buffer_empty()) // no data to read 
        if (filp->f_flags & O_NONBLOCK) 
            // if nonblock is specified, we dont block and just return 'try again'
            return -EAGAIN; 
 
    unsigned int i;
    i = 0;
    /*
    DECLARE_WAITQUEUE(wait, current); // contains a pointer to a task struct 
 
    if (is_buffer_empty()) // no data to read 
      if (filp->f_flags & O_NONBLOCK) 
     return -EAGAIN;
     
    add_wait_queue(&qin, &wait); // Link our wait struct to the qin waitq 
 
    while(is_buffer_empty()){
        printk(" %s read - blocking \r\n", my_devname);
        set_current_state(TASK_INTERRUPTIBLE);
        if(!is_buffer_empty()) // There is a data in device 
            break;  
        schedule();
        if(signal_pending(current)) {
           set_current_state(TASK_RUNNING);
           remove_wait_queue(&qin,&wait);
           return -ERESTARTSYS;
        } // signal 
    } // while 
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&qin,&wait);
     */
 
 
    /* equivalent to what's above */
    printk(" read going to sleep - %s\n", my_devname);
    wait_event_interruptible(qin, !is_buffer_empty()); //could check return value to 
                                                       //see if interrupted by signal
    printk(" read woke up - %s\n", my_devname);
 
    // checking if we were woken up by a signal (we used wait_event_interruptible)
    if (signal_pending(current))
    {
        printk(" signal pending in read %s\n", my_devname);  
        return -ERESTARTSYS; 
        // value used by VFS. It will either restart syscall, or return -EINTR to user space
    }
     
    if (down_interruptible(&sema))
    {
        printk("semaphore down in read %s\n", my_devname);
        return -ERESTARTSYS;
    }
 
    // there is data to read
    while(i<len){
        if(ir==iw) 
            break;
        put_user(rbuf[ir++], buffer++);
        i++; 
        if(ir==N)  // end of device, reset the ir pointer to begining
            ir=0;
    }
    printk("\n %s read - %d bytes \r\n", my_devname, i);
 
    up(&sema);
 
    // add code to wake up writers. Writers are not blocked in this version 
    printk(" read is trying to wake up writers - %s\n", my_devname);
    wake_up_interruptible(&qout);
 
    return i;
}
 
static ssize_t device_write(struct file *filp, 
                            const char *buffer, size_t len, loff_t *offs)
{
    unsigned int i=0;
 
    //add code to put a process to sleep if no room to write
 
    if (is_buffer_full()) /* no room to write */
      if (filp->f_flags & O_NONBLOCK)
         return -EAGAIN;
 
    printk(" write going to sleep - %s\n", my_devname);
    wait_event_interruptible(qout, !is_buffer_full());
    printk(" write woke up - %s\n", my_devname);
 
    if (signal_pending(current))
    {
        printk("write was woken by signal %s\n", my_devname);
        return -ERESTARTSYS;
    }
     
    if (down_interruptible(&sema))
    {   
        printk("write has semaphore down %s\n", my_devname); 
        return -ERESTARTSYS;
    }
 
 
    // let's write something
    while(i<len)
    {
        if(iw==ih)
            break;
        get_user(rbuf[iw++], buffer++);
        i++;
        if(iw==N)  // end of device, reset the ir pointer to begining
            iw = 0;
    }
    printk("\n %s write - %d bytes \r\n", my_devname, i);
     
    up(&sema); // should semaphore be just before return???
 
    // done writing, so waking up readers 
    printk(" write is trying to wake up readers - %s\n", my_devname);
    wake_up_interruptible(&qin);
     
    return i;
}
 
static int device_open(struct inode *inode, struct file *filp)
{
  int use;
 
  spin_lock(&lock);
  if( use_count == 0 )
  {
    ir=iw=0;
  }
  use_count++; use=use_count;
  spin_unlock(&lock);
 
  printk(" %s open - count: %d \r\n", my_devname, use);
 
  return 0;
}
 
static int device_release(struct inode *inode, struct file *filp)
{
  int use;
 
  spin_lock(&lock);
  use = --use_count;
  spin_unlock(&lock);
 
  printk(" %s close - count %d \r\n", my_devname, use);
 
  return 0;
} 
 
//......................................................
 
static struct file_operations fops = {
  owner:   THIS_MODULE,
  read:    device_read,       
  write:   device_write,
  poll:    device_poll,
  open:    device_open,
  release: device_release,
};
 
static        dev_t   dev;
static struct cdev   *my_cdev;
 
unsigned int wait_major;
unsigned int wait_minor;
 
static int __init my_init(void)
{
  int res=0, major, minor;
 
  sema_init(&sema,1); /* initialize usage count as 1, acts like mutex */
 
  /*  static device major and minor
  wait_major = DEV_MAJOR;
  wait_minor = DEV_MINOR;
  dev = MKDEV(wait_major, wait_minor);
  res = register_chrdev(dev,my_devname,&fops);
 
  printk("The device is registered by Major no: %d\n",res);
  if(res == -1)
  {
    printk("Error in registering the module\n");
    return -1;
  }
*/
 
    /* Dynamic major number */
    res = alloc_chrdev_region(&dev, 0, 1, my_devname);
    if (res<0) 
    {   
        printk("Cannot get major number - %s \n", my_devname);
        return res; 
    }
 
    major = MAJOR(dev); /* device major number */
    minor = MINOR(dev);
    printk("Device %s got major number %d and minor number %d\n", my_devname, major, minor);
 
    /* allocate cdev structure and point to our device fops*/
    my_cdev = cdev_alloc();
    cdev_init(my_cdev, &fops);
    my_cdev->owner = THIS_MODULE;
 
    /* connect major to cdev struct */
    res = cdev_add(my_cdev, dev, 1);
    if (res<0)
    {
        printk("Couldnt connect major number to cdev structure - %s\n", my_devname);
        unregister_chrdev_region(dev, 1);
        return res;
    }
    use_count = 0;  /* initialize use count */
 
    /* dynamic device node creation */
    poll_class = class_create(THIS_MODULE, my_devname);
    device_create(poll_class, NULL, dev, NULL, my_devname, 1); 
    printk("Created device node %s\n", my_devname);
 
    return res;
}
 
static void __exit my_exit(void)
{
  cdev_del(my_cdev);
  unregister_chrdev_region(dev, 1);
   
  // For static device nodes, use unregister_chrdev(wait_major,my_devname);
  
  device_destroy(poll_class, dev);
  class_destroy(poll_class);
  printk(" %s remove \r\n", my_devname);
}
 
module_init(my_init);
module_exit(my_exit);
 
MODULE_AUTHOR("Jay Medina");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blocking IO and poll");
