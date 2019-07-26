#ifndef GPD_DEV_H
#define GPD_DEV_H

#include "gpd_util.h"

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id);
int device_init(struct pci_dev *pdev);
void device_remove(struct pci_dev *pdev);

#endif
