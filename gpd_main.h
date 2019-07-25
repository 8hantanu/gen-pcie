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

static const char gpd_name[] = "GenericPCIeDriver";

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id);
int device_init(struct pci_dev *pdev);
void device_remove(struct pci_dev *pdev);

struct pci_device_id pci_dev_id_gpd[] = {
    {VENDOR_ID, DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
    {}
};

struct pci_driver gen_pcie_driv = {
    .name = (char *) gpd_name,
    .id_table = pci_dev_id_gpd,
    .probe = device_probe,
    .remove = device_remove
};
