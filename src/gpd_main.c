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
    .probe = dev_probe,
	.sriov_configure = dev_sriov_configure,
    .remove = dev_remove
};

static int __init gpd_init_module(void) {

    int err;

    // Allocate one minor number per domain
	err = alloc_chrdev_region(&gpd_dev_num, 0, MAX_NUM_DEV_FILES, gpd_name);

    // cdev class
	dev_class = class_create(THIS_MODULE, "dev_class");

    // Calls the probe function of gen_pcie_driv and registers all devices(pf and vf)
    if (pci_register_driver(&gen_pcie_driv))
        GPD_ERR("Driver register failed");
    return 0;
}

static void __exit gpd_exit_module(void) {
    // Calls the remove function on gen_pcie_driv
    pci_unregister_driver(&gen_pcie_driv);
}

module_init(gpd_init_module);
module_exit(gpd_exit_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shantanu.mishra@intel.com");
MODULE_DESCRIPTION("Generic PCIe Driver");
MODULE_VERSION("0.0.1");
