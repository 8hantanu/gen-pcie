#ifndef GPD_FOPS_H
#define GPD_FOPS_H

#include "gpd_util.h"
#include "gpd_ioctl.h"

// For storing BAR space head pointers
struct gpd_hw {
    /* BAR 0 address */
    void __iomem *csr_kva;
    unsigned long csr_phys_addr;
    /* BAR 2 address */
    void __iomem *func_kva;
    unsigned long func_phys_addr;
};

// Driver data container
// Contains pointers to pci_dev and cdev
// Associated with pdev using pci_set_drvdata()
struct gpd_dev {
    int id;
    struct pci_dev *pdev;
    struct cdev cdev;
    struct gpd_hw hw;
    dev_t dev_number;
    struct list_head list;
    struct device *gpd_device;
};

/**
 * Invoked when open("/dev/<dev_name>", O_RDWR) is called
 * @param  i inode pointer
 * @param  f file pointer
 * @return   0 if successful
 */
int gpd_open(struct inode *i, struct file *f);

// int gpd_close(struct inode *i, struct file *f);

// ssize_t gpd_read(struct file *f, char __user *buf, size_t len, loff_t *offset);

// ssize_t gpd_write(struct file *f, const char __user *buf, size_t len, loff_t *offset);

// int gpd_mmap(struct file *f, struct vm_area_struct *vma);


#if KERNEL_VERSION(2, 6, 35) <= LINUX_VERSION_CODE
  /**
   * Defines IOCTL call functions for different commands
   * @param  f   file pointer
   * @param  cmd IOCTL command number
   * @param  arg pointer to user var
   * @return     0 if successful
   */
  long gpd_ioctl(struct file *f, unsigned int cmd, unsigned long arg);
#else
  /**
   * Defines IOCTL call functions for different commands
   * @param  i   inode pointer
   * @param  f   file pointer
   * @param  cmd IOCTL command number
   * @param  arg pointer to user var
   * @return     0 if successful
   */
  int gpd_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg);
#endif

#endif
