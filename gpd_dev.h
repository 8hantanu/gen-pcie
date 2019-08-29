#ifndef GPD_DEV_H
#define GPD_DEV_H

#include "gpd_util.h"
#include "gpd_fops.h"

struct q_head {
	void *base;
	dma_addr_t dma_base;
};

extern dev_t gpd_dev_num;
extern struct class *dev_class;
extern struct q_head q_heads[NUM_DIR_QS];

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id);
int device_init(struct gpd_dev *gpd_dev, struct pci_dev *pdev);
int device_map_bar_space(struct gpd_dev *gpd_dev, struct pci_dev *pdev);
int device_cdev_add(struct gpd_dev *gpd_dev, dev_t base, const struct file_operations *fops);
int device_pf_create(struct gpd_dev *gpd_dev, struct pci_dev *pdev, struct class *dev_class);
int device_alloc_dma_coherent(struct gpd_dev *gpd_dev);
int device_sriov_configure(struct pci_dev *pdev, int num_vfs);
void device_remove(struct pci_dev *pdev);

#endif
