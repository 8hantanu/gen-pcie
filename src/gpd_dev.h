#ifndef GPD_DEV_H
#define GPD_DEV_H

#include "gpd_util.h"
#include "gpd_fops.h"

extern dev_t gpd_dev_num;
extern struct class *dev_class;

int dev_probe(struct pci_dev *pdev, const struct pci_device_id *id);
int dev_init(struct gpd_dev *gpd_dev);
int dev_map_bar_space(struct gpd_dev *gpd_dev);
int dev_add_cdev(struct gpd_dev *gpd_dev, dev_t base, const struct file_operations *fops);
int dev_pf_create(struct gpd_dev *gpd_dev, struct class *dev_class);
int dev_alloc_dma_coherent(struct gpd_dev *gpd_dev);
void dev_mask_uerr(struct pci_dev *pdev);
int dev_function_reset(struct pci_dev *pdev, bool save_state);
int dev_sriov_configure(struct pci_dev *pdev, int num_vfs);
void dev_remove(struct pci_dev *pdev);

#endif
