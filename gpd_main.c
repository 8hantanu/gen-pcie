#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/aer.h>

#define GPD_LOG(string...) printk(KERN_NOTICE string)
#define GPD_ERR(string...) printk(KERN_ERR string)

#define VENDOR_ID       0x8086
#define DEVICE_ID       0x270b

MODULE_LICENSE("GPL");

static const char gpd_name[] = "GenPCIeDriv";

struct pci_dev *pci_dev;
unsigned long mmio_addr;
unsigned long reg_len;
unsigned long *base_addr;

int device_probe(struct pci_dev *dev, const struct pci_device_id *id);
int device_init(struct pci_dev *dev);
void device_remove(struct pci_dev *dev);

struct pci_device_id  pci_dev_id_gpd[] = {
    {VENDOR_ID, DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
    {}
};

struct pci_driver gen_pcie_driv = {
    .name = (char *) gpd_name,
    .id_table = pci_dev_id_gpd,
    .probe = device_probe,
    .remove = device_remove
};

static int __init gpd_init_module(void) {
    if (pci_register_driver(&gen_pcie_driv))
        GPD_ERR("Driver register failed");
    return 0;
}

static void __exit gpd_exit_module(void) {
    pci_unregister_driver(&gen_pcie_driv);
}

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id) {

    GPD_LOG("Device probed");

    if (pci_enable_device(pdev) != 0 )
        GPD_ERR("Failed while enabling");

    pci_dev = pci_get_device(VENDOR_ID, DEVICE_ID, NULL);
    if (pci_dev)
        GPD_LOG("Device found with vendor ID %d and devive ID %d", VENDOR_ID, DEVICE_ID);
    else
        GPD_ERR("Device not found");

    device_init(pci_dev);

    return 0;
}

int device_init(struct pci_dev *pdev) {
    if (pci_request_regions(pdev, gpd_name))
        GPD_ERR("Region map failed");

    pci_set_master(pdev);

    pci_intx(pdev, 0);

	pci_enable_pcie_error_reporting(pdev);

    dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

    return 0;
}

void device_remove(struct pci_dev *dev) {
    pci_release_regions(dev);
    pci_disable_device(dev);
}

module_init(gpd_init_module);
module_exit(gpd_exit_module);
