#include "gpd_main.h"

MODULE_LICENSE("GPL");

static const char gpd_name[] = "GenPCIeDriv";

struct pci_dev *pci_dev;
struct msix_entry *entries;
unsigned long mmio_addr;
unsigned long reg_len;
unsigned long *base_addr;

int device_probe(struct pci_dev *dev, const struct pci_device_id *id);
int device_init(struct pci_dev *dev);
void device_remove(struct pci_dev *dev);

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

static int __init gpd_init_module(void) {
    if (pci_register_driver(&gen_pcie_driv))
        GPD_ERR("Driver register failed");
    return 0;
}

static void __exit gpd_exit_module(void) {
    pci_unregister_driver(&gen_pcie_driv);
}

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id) {
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
    GPD_LOG("Enabled bus master");
    pci_intx(pdev, 0);
    pci_enable_msix_range(pdev, entries, 1, pci_msix_vec_count(pdev));
    GPD_LOG("Enabled interrupts");
	pci_enable_pcie_error_reporting(pdev);
    // not recomm
    pci_set_power_state(pdev, PCI_D3hot);
    return 0;
}

void device_remove(struct pci_dev *pdev) {
    pci_release_regions(pdev);
    GPD_LOG("Released mapped regions");
    pci_disable_device(pdev);
    GPD_LOG("Disabled device");
}

module_init(gpd_init_module);
module_exit(gpd_exit_module);
