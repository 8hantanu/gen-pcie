#include "gpd_dev.h"
#include "gpd_sys.h"
#include "gpd_sriov.h"

struct pci_dev *pci_dev;
struct msix_entry *entries;

struct file_operations gpd_fops = {
	.owner   = THIS_MODULE,
//	.open    = gpd_open,
//	.release = gpd_close,
///	.read    = gpd_read,
	.write   = gpd_write,
//	.mmap    = gpd_mmap,
#if KERNEL_VERSION(2, 6, 35) <= LINUX_VERSION_CODE
//	.unlocked_ioctl = gpd_ioctl,
#else
//	.ioctl   = gpd_ioctl,
#endif

};

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id) {

    struct gpd_dev *gpd_dev;
	int ret;
	gpd_dev = devm_kcalloc(&pdev->dev, 1, sizeof(struct gpd_dev), GFP_KERNEL);
	if (!gpd_dev) {
		ret = -ENOMEM;
	}

    pci_set_drvdata(pdev, gpd_dev);

	gpd_dev->pdev = pdev;

    // Find the device using Device and Vendor ID
    pci_dev = pci_get_device(VENDOR_ID, DEVICE_ID, NULL);
    if (pci_dev) {
        GPD_LOG("Device found");
        // Reset device before init caps
        if (pcie_device_reset(pci_dev, true))
            GPD_ERR("Function reset failed");
        else
            GPD_LOG("Reset device");
        device_init(gpd_dev, pci_dev);
    } else {
        GPD_ERR("Device not found");
    }

    return 0;
}



int device_init(struct gpd_dev *gpd_dev, struct pci_dev *pdev) {

    if (pci_enable_device(pdev) != 0 )
        GPD_ERR("Failed while enabling");

    // TEST
    // device_sriov_configure(pdev, 2);

    // Enable bus mastering
    pci_set_master(pdev);
    GPD_LOG("Enabled bus mastering");

    // Request regions addressed by the BARs
    if (pci_request_regions(pdev, gpd_name))
        GPD_ERR("MMIO/IOP map failed");
    else
        GPD_LOG("Mapped MMIO/IOP regions");

    // Set DMA Mask
    dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

    // Enable PCI INTx
    pci_intx(pdev, 0);

    // Enable interrupt vectors
	int vecs_available = pci_msix_vec_count(pdev);
	int vecs_assigned = pci_enable_msix_range(pdev, entries, 1, vecs_available);
    if (vecs_available != vecs_assigned)
        GPD_ERR("MSI-X vectors enable failed");
    else
        GPD_LOG("Enabled MSI-X vectors");
	printk(KERN_NOTICE "GPD: Total %d MSI vecs available", vecs_available);
	printk(KERN_NOTICE "GPD: Total %d MSI vecs assigned", vecs_assigned);

    pci_alloc_irq_vectors(pdev, 9, 9, PCI_IRQ_MSIX);

    // UEMsk
    pcie_mask_uerr(pdev);

    // Enable AER capability
    if (pci_enable_pcie_error_reporting(pdev))
        GPD_ERR("AER enable failed");
    else
        GPD_LOG("Enabled AER");

    // BAR iomap
	gpd_dev->hw.csr_kva = pci_iomap(pdev, 0, 0);
	gpd_dev->hw.csr_phys_addr = pci_resource_start(pdev, 0);
	if (!gpd_dev->hw.csr_kva) {
        GPD_ERR("Failed to map BAR 0");
        // TODO: Add fail flow
    } else {
        GPD_LOG("Mapped BAR 0 space");
        printk(KERN_NOTICE "GPD: BAR 0 start at 0x%llx\n", pci_resource_start(pdev, 0));
        printk(KERN_NOTICE "GPD: BAR 0 len is %llu\n", pci_resource_len(pdev, 0));
    }
	gpd_dev->hw.func_kva = pci_iomap(pdev, 2, 0);
	gpd_dev->hw.func_phys_addr = pci_resource_start(pdev, 2);
    if (!gpd_dev->hw.func_kva) {
        GPD_ERR("Failed to map BAR 2");
        // TODO: Add fail flow
    } else {
        GPD_LOG("Mapped BAR 2 space");
        printk(KERN_NOTICE "GPD: BAR 2 start at 0x%llx\n", pci_resource_start(pdev, 2));
        printk(KERN_NOTICE "GPD: BAR 2 len is %llu\n", pci_resource_len(pdev, 2));
    }

    // Add cdev
    device_cdev_add(gpd_dev, gpd_dev_num, &gpd_fops);

    // Create cdev
    device_pf_create(gpd_dev, pdev, dev_class);

    // Set device power state to D3
    // pci_set_power_state(pdev, PCI_D3hot);

    // Cleans up uncorrectable error status registers
    pci_cleanup_aer_uncorrect_error_status(pdev);

    return 0;
}


int device_cdev_add(struct gpd_dev *gpd_dev,
		    dev_t base,
		    const struct file_operations *fops)
{
	int ret;

	gpd_dev->dev_number = MKDEV(MAJOR(base), MINOR(base) + (gpd_dev->id * NUM_DEV_FILES_PER_DEVICE));

	cdev_init(&gpd_dev->cdev, fops);

	gpd_dev->cdev.dev   = gpd_dev->dev_number;
	gpd_dev->cdev.owner = THIS_MODULE;

	ret = cdev_add(&gpd_dev->cdev,
		       gpd_dev->cdev.dev,
		       NUM_DEV_FILES_PER_DEVICE);

    if (ret < 0)
	   GPD_ERR("Add cdev failed");

	return ret;
}


int device_pf_create(struct gpd_dev *gpd_dev,
			 struct pci_dev *pdev,
			 struct class *dev_class)
{
	dev_t dev;

	dev = MKDEV(MAJOR(gpd_dev->dev_number), MINOR(gpd_dev->dev_number) + MAX_NUM_DOMAINS);

	/* Create a new device in order to create a /dev/ gpd node. This device
	 * is a child of the HQM PCI device.
	 */
	gpd_dev->gpd_device = device_create(dev_class,
					    &pdev->dev,
					    dev,
					    gpd_dev,
					    "gpd%d/gpd",
					    gpd_dev->id);

	return 0;
}


int device_sriov_configure(struct pci_dev *pdev, int num_vfs) {
    if (num_vfs)
        return device_sriov_enable(pdev, num_vfs);
    else
        return device_sriov_disable(pdev, num_vfs);
}


void device_remove(struct pci_dev *pdev) {

	pci_free_irq_vectors(pdev);
    GPD_LOG("Released IRQ vectors");

	pci_disable_msix(pdev);
	GPD_LOG("Released MSI-X vectors");

    pci_disable_pcie_error_reporting(pdev);
    GPD_LOG("Disabled AER");

    pci_release_regions(pdev);
    GPD_LOG("Released MMIO/IOP regions");

    pci_clear_master(pdev);
    GPD_LOG("Disabled DMA");

    pci_disable_device(pdev);
    GPD_LOG("Disabled device");
}
