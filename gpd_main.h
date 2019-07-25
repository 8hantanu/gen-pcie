#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/aer.h>

#define GPD_LOG(string...) printk(KERN_NOTICE "GPD: %s", string)
#define GPD_ERR(string...) printk(KERN_ERR "GPD: %s", string)

#define VENDOR_ID       0x8086
#define DEVICE_ID       0x270b
