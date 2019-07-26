#include "gpd_sys.h"

void pcie_mask_uerr(struct pci_dev *dev) {

    u32 mask;

	int pos = pci_find_ext_capability(dev, PCI_EXT_CAP_ID_ERR);

	pci_read_config_dword(dev, pos + PCI_ERR_UNCOR_MASK, &mask);

	mask |= PCI_ERR_UNC_UNSUP;

	pci_write_config_dword(dev, pos + PCI_ERR_UNCOR_MASK, mask);
}

//
// bool pcie_has_flr(struct pci_dev *dev) {
//
// 	u32 cap;
//
// 	if (dev->dev_flags & PCI_DEV_FLAGS_NO_FLR_RESET)
// 		return false;
//
// 	pcie_capability_read_dword(dev, PCI_EXP_DEVCAP, &cap);
//
// 	return cap & PCI_EXP_DEVCAP_FLR;
// }
//
//
// int pcie_flr(struct pci_dev *dev) {
//
// 	if (!pci_wait_for_pending_transaction(dev))
// 		pci_err(dev, "timed out waiting for pending transaction; performing function level reset anyway\n");
//
// 	pcie_capability_set_word(dev, PCI_EXP_DEVCTL, PCI_EXP_DEVCTL_BCR_FLR);
//
// 	if (dev->imm_ready)
// 		return 0;
//
// 	/*
// 	 * Per PCIe r4.0, sec 6.6.2, a device must complete an FLR within
// 	 * 100ms, but may silently discard requests while the FLR is in
// 	 * progress.  Wait 100ms before trying to access the device.
// 	 */
// 	msleep(100);
// 	return pci_dev_wait(dev, "FLR", PCIE_RESET_READY_POLL_MS);
// }
