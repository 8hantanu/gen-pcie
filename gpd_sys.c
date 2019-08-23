#include "gpd_sys.h"

void pcie_mask_uerr(struct pci_dev *pdev) {

    u32 mask;

    int pos = pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ERR);

    pci_read_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, &mask);

    mask |= PCI_ERR_UNC_UNSUP;

    pci_write_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, mask);
}


int pcie_device_reset(struct pci_dev *pdev, bool save_state) {

    int ret;

    /* TODO: When upstreaming this, replace this function with
     * pci_reset_function_locked() (available in newer kernels).
     */

    if (save_state) {
        ret = pci_save_state(pdev);
        if (ret)
            return ret;
    }

    // NOTE: Can use pci_reset_function_locked(pdev) instead in new kernels
    ret = __pci_reset_function_locked(pdev);
    if (ret)
        return ret;

    if (save_state)
        pci_restore_state(pdev);

    return 0;
}
