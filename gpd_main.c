#include "gpd_dev.h"

unsigned long mmio_addr;
unsigned long reg_len;
unsigned long *base_addr;

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
    // Calls the probe function of gen_pcie_driv and registers all devices
    if (pci_register_driver(&gen_pcie_driv))
        GPD_ERR("Driver register failed");
    return 0;
}

static void __exit gpd_exit_module(void) {
    pci_unregister_driver(&gen_pcie_driv);
}

module_init(gpd_init_module);
module_exit(gpd_exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shantanu.mishra@intel.com");
MODULE_DESCRIPTION("Generic PCIe Driver");
MODULE_VERSION("0.0.1");
