#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>         // kmalloc()
#include <linux/uaccess.h>      // copy_to/from_user()


#define mem_size    1024        // kernel buffer size

dev_t dev = 0;
static struct class *dev_class;
static struct cdev gpd_cdev;
uint8_t *kernel_buffer;
struct pci_dev *pci_dev;

static int __init gpd_driver_init(void);
static void __exit gpd_driver_exit(void);
static int gpd_open(struct inode *inode, struct file *file);
static int gpd_release(struct inode *inode, struct file *file);
static ssize_t gpd_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t gpd_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = gpd_read,
    .write      = gpd_write,
    .open       = gpd_open,
    .release    = gpd_release,
};

static int gpd_open(struct inode *inode, struct file *file)
{


    /*Creating Physical memory*/
    if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL))) {
        printk(KERN_INFO "Device file opened\n");
        return 0;
    } else {
        printk(KERN_INFO "Cannot allocate memory in kernel\n");
        return -1;
    }
}

static int gpd_release(struct inode *inode, struct file *file)
{
    kfree(kernel_buffer);
    printk(KERN_INFO "Device file closed\n");
    return 0;
}

static ssize_t gpd_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    if (copy_to_user(buf, kernel_buffer, mem_size)){
        printk(KERN_INFO "Data read failed\n");;
    }
    printk(KERN_INFO "Data read done\n");
    return mem_size;
}
static ssize_t gpd_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    if (copy_from_user(kernel_buffer, buf, len)){
        printk(KERN_INFO "Data write failed\n");
    }
    printk(KERN_INFO "Data write done\n");
    return len;
}

static int __init gpd_driver_init(void)
{
    /* Allocating Major number */
    if((alloc_chrdev_region(&dev, 0, 1, "gpd_Dev")) < 0){
        printk(KERN_INFO "Cannot allocate major number\n");
        return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /* Creating cdev structure */
    cdev_init(&gpd_cdev,&fops);

    /* Adding character device to the system */
    if(cdev_add(&gpd_cdev,dev,1) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }

    /* Creating struct class */
    if((dev_class = class_create(THIS_MODULE,"gpd_class"))){
        printk(KERN_INFO "Created device struct class\n");
    } else {
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }

    /* Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"gpd_device"))){
        printk(KERN_INFO "Created device I\n");
    } else {
        printk(KERN_INFO "Cannot create the device I\n");
        goto r_device;
    }
    printk(KERN_INFO "Device driver inserted\n");
    return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}

void __exit gpd_driver_exit(void)
{
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&gpd_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device driver removed\n");
}

module_init(gpd_driver_init);
module_exit(gpd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shantanu.mishra@intel.com");
