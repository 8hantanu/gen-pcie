#include "gpd_main.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shantanu.mishra@intel.com");

struct pci_dev *pci_dev;
struct msix_entry *entries;
unsigned long mmio_addr;
unsigned long reg_len;
unsigned long *base_addr;

static void pcie_mask_uerr(struct pci_dev *dev);
static int pcie_flr(struct pci_dev *dev);


static int __init gpd_init_module(void) {
    // Calls the probe function and registers all devices
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

    // Find the device using Device and Vendor ID
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

    // Initiate function level reset if available
    pcie_flr(pdev);
    GPD_LOG("Initiated FLR");

    // Request region addressed by the BARs
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
    pcie_mask_uerr(pdev);

    // Enable AER capability
	if (pci_enable_pcie_error_reporting(pdev))
        GPD_ERR("AER enable failed");
    else
        GPD_LOG("Enabled AER");

    // Set device power state to D3
    pci_set_power_state(pdev, PCI_D3hot);

    // Cleans up uncorrectable error status registers
    // pci_cleanup_aer_uncorrect_error_status(pdev);

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



static void pcie_mask_uerr(struct pci_dev *dev) {

    u32 mask;

	int pos = pci_find_ext_capability(dev, PCI_EXT_CAP_ID_ERR);

	pci_read_config_dword(dev, pos + PCI_ERR_UNCOR_MASK, &mask);

	mask |= PCI_ERR_UNC_UNSUP;

	pci_write_config_dword(dev, pos + PCI_ERR_UNCOR_MASK, mask);
}


static int pcie_flr(struct pci_dev *dev) {

	u32 cap;

	pcie_capability_read_dword(dev, PCI_EXP_DEVCAP, &cap);
	if (!(cap & PCI_EXP_DEVCAP_FLR))
		return -ENOTTY;

	if (!pci_wait_for_pending_transaction(dev))
		dev_err(&dev->dev, "timed out waiting for pending transaction; performing function level reset anyway\n");

	pcie_capability_set_word(dev, PCI_EXP_DEVCTL, PCI_EXP_DEVCTL_BCR_FLR);
	// usleep(100e3);
	return 0;
}


module_init(gpd_init_module);
module_exit(gpd_exit_module);
