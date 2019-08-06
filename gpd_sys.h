#ifndef GPD_SYS_H
#define GPD_SYS_H

#include "gpd_util.h"

void pcie_mask_uerr(struct pci_dev *pdev);
int pcie_device_reset(struct pci_dev *pdev, bool save_state);

#endif
