#include "gpd_main.h"

MODULE_LICENSE("GPL");

static const char gpd_name[] = "GenericPCIeDriver";
struct pci_dev *pci_dev;
struct msix_entry *entries;
unsigned long mmio_addr;
unsigned long reg_len;
unsigned long *base_addr;

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
    if (pci_dev) {
        GPD_LOG("Device found");
        device_init(pci_dev);
    } else {
        GPD_ERR("Device not found");
    }

    return 0;
}


int device_init(struct pci_dev *pdev) {

    if (pci_request_regions(pdev, gpd_name))
        GPD_ERR("Region map failed");
    else
        GPD_LOG("Mapped memory regions");

    // Enable bus mastering
    pci_set_master(pdev);
    GPD_LOG("Enabled bus mastering");

    // Enable PCI INTx for device
    pci_intx(pdev, 0);

    // Enable MSI-X vectors
    if (pci_msix_vec_count(pdev) != pci_enable_msix_range(pdev, entries, 1, pci_msix_vec_count(pdev)))
        GPD_ERR("MSI-X vectors enable failed");
    else
        GPD_LOG("Enabled MSI-X vectors");

    // UEMsk
    u32 mask;
    // Request address for Advanced Error Reporting capability
	int pos = pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ERR);
	pci_read_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, &mask);
	mask |= PCI_ERR_UNC_UNSUP;
	pci_write_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, mask);

    // Enable AER capability
	if (pci_enable_pcie_error_reporting(pdev))
        GPD_ERR("AER enable failed");
    else
        GPD_LOG("Enabled AER");

    // not recomm
    pci_set_power_state(pdev, PCI_D3hot);

    return 0;
}


void device_remove(struct pci_dev *pdev) {

    pci_disable_pcie_error_reporting(pdev);
    GPD_LOG("Disabled AER");

    pci_release_regions(pdev);
    GPD_LOG("Released mapped regions");

    pci_disable_device(pdev);
    GPD_LOG("Disabled device");
}

module_init(gpd_init_module);
module_exit(gpd_exit_module);
