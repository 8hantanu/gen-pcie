#include "gpd_dev.h"
#include "gpd_sys.h"

struct pci_dev *pci_dev;
struct msix_entry *entries;

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
    // pcie_flr(pdev);
    // GPD_LOG("Initiated FLR");

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
