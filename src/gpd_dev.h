#ifndef GPD_DEV_H
#define GPD_DEV_H

#include "gpd_util.h"
#include "gpd_fops.h"

// Major and minor number for char dev
extern dev_t gpd_dev_num;

// Class for char dev
extern struct class *dev_class;

/**
 * Probes all devices(physical and virtual) mentioned in pci_device_id
 * Invoked when pci_register_driver() is called
 * @param  pdev pci_dev pointer
 * @param  id   pci dev id
 * @return      0 if successful
 */
int dev_probe(struct pci_dev *pdev, const struct pci_device_id *id);

/**
 * Enaables and initializes basic PCIe device features and capabilities
 * @param  gpd_dev driver data container pointer
 * @return         0 if successful
 */
int dev_init(struct gpd_dev *gpd_dev);

/**
 * Maps BAR address spaces for BAR 0 and BAR 2
 * @param  gpd_dev driver data container pointer
 * @return         0 if successful
 */
int dev_map_bar_space(struct gpd_dev *gpd_dev);

/**
 * Creates char device and assigns file operations
 * @param  gpd_dev driver data container pointer
 * @param  base    major minor number
 * @param  fops    file operations
 * @return         0 if successful
 */
int dev_add_cdev(struct gpd_dev *gpd_dev, dev_t base, const struct file_operations *fops);

/**
 * Registers created char dev with major minor number
 * @param  gpd_dev   driver data container pointer
 * @param  dev_class class for char dev
 * @return           0 if successful
 */
int dev_pf_create(struct gpd_dev *gpd_dev, struct class *dev_class);

/**
 * Allocates DMA coherent memory
 * @param  gpd_dev driver data container pointer
 * @return         0 if successful
 */
int dev_alloc_dma_coherent(struct gpd_dev *gpd_dev);

/**
 * Enables masking of uncorrectable errors
 * @param pdev pci_dev pointer
 */
void dev_mask_uerr(struct pci_dev *pdev);

/**
 * Resets device with an option to save PCIe config space
 * @param  pdev       pci_dev pointer
 * @param  save_state set true to save PCIe config space
 * @return            0 if successful
 */
int dev_function_reset(struct pci_dev *pdev, bool save_state);

/**
 * Enables SR-IOV and invokes VFs creation
 * @param  pdev    pci_dev pointer
 * @param  num_vfs number of virtual functions to be created
 *                 set 0 to disable SR-IOV
 * @return         0 if successful
 */
int dev_sriov_configure(struct pci_dev *pdev, int num_vfs);

/**
 * Disables features and deallocates any memory assigned
 * Invoked when pci_unregister_driver() is called
 * @param pdev pci_dev pointer
 */
void dev_remove(struct pci_dev *pdev);

#endif
