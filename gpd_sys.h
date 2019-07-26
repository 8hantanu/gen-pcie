#ifndef GPD_SYS_H
#define GPD_SYS_H

#include "gpd_main.h"

void pcie_mask_uerr(struct pci_dev *dev);
bool pcie_has_flr(struct pci_dev *dev);
int pcie_flr(struct pci_dev *dev);

#endif
