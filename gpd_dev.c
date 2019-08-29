#include "gpd_dev.h"
#include "gpd_sys.h"
#include "gpd_sriov.h"

struct pci_dev *pci_dev;
struct msix_entry *entries;

struct file_operations gpd_fops = {
    .owner   = THIS_MODULE,
//  .open    = gpd_open,
//  .release = gpd_close,
//  .read    = gpd_read,
    .write   = gpd_write,
//  .mmap    = gpd_mmap,
// #if KERNEL_VERSION(2, 6, 35) <= LINUX_VERSION_CODE
//  .unlocked_ioctl = gpd_ioctl,
// #else
//  .ioctl   = gpd_ioctl,
// #endif
};

int i;

dev_t gpd_dev_num;
struct class *dev_class;

struct q_head q_heads[NUM_DIR_QS];

int device_probe(struct pci_dev *pdev, const struct pci_device_id *id) {

    struct gpd_dev *gpd_dev;
    gpd_dev = devm_kcalloc(&pdev->dev, 1, sizeof(struct gpd_dev), GFP_KERNEL);
    if (!gpd_dev)
        return -ENOMEM;

    pci_set_drvdata(pdev, gpd_dev);

    gpd_dev->pdev = pdev;

    if (pcie_device_reset(pdev, true))
        GPD_ERR("Function reset failed");
    else
        GPD_LOG("Device Reset");

    device_init(gpd_dev, pdev);

    return 0;
}


int device_init(struct gpd_dev *gpd_dev, struct pci_dev *pdev) {

    if (pci_enable_device(pdev) != 0 )
        GPD_ERR("Failed while enabling");

    // VFs
    // device_sriov_configure(pdev, 2);

    // Request regions addressed by the BARs
    if (pci_request_regions(pdev, gpd_name))
        GPD_ERR("MMIO/IOP map failed");
    else
        GPD_LOG("Mapped MMIO/IOP regions");

    // Enable bus mastering
    pci_set_master(pdev);
    GPD_LOG("Enabled bus mastering");

    // Enable PCI INTx
    pci_intx(pdev, 0);

    // Enable AER capability
    if (pci_enable_pcie_error_reporting(pdev))
        GPD_ERR("AER enable failed");
    else
        GPD_LOG("Enabled AER");

    // Set DMA Mask
    dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

    // Enable interrupt vectors
    // int vecs_available = pci_msix_vec_count(pdev);
    // int vecs_assigned = pci_enable_msix_range(pdev, entries, 1, vecs_available);
    // if (vecs_available != vecs_assigned)
    //     GPD_ERR("MSI-X vectors enable failed");
    // else
    //     GPD_LOG("Enabled MSI-X vectors");
    // printk(KERN_NOTICE "GPD: Total %d MSI vecs available", vecs_available);
    // printk(KERN_NOTICE "GPD: Total %d MSI vecs assigned", vecs_assigned);
    //
    // pci_alloc_irq_vectors(pdev, 9, 9, PCI_IRQ_MSIX);

    // UEMsk
    pcie_mask_uerr(pdev);

    // BAR iomap
    device_map_bar_space(gpd_dev, pdev);

    // Add cdev
    device_cdev_add(gpd_dev, gpd_dev_num, &gpd_fops);

    // Create cdev
    device_pf_create(gpd_dev, pdev, dev_class);

    // Set device power state to D3
    // pci_set_power_state(pdev, PCI_D3hot);

    // Allocate DMA coherent
    device_alloc_dma_coherent(gpd_dev);

    // Cleans up uncorrectable error status registers
    pci_cleanup_aer_uncorrect_error_status(pdev);

    return 0;
}


int device_map_bar_space(struct gpd_dev *gpd_dev, struct pci_dev *pdev) {

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

    return 0;
}


int device_cdev_add(struct gpd_dev *gpd_dev,
            dev_t base,
            const struct file_operations *fops)
{
    int ret;

    gpd_dev->dev_number = MKDEV(MAJOR(base), MINOR(base));// + (gpd_dev->id * NUM_DEV_FILES_PER_DEVICE));

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

    dev = MKDEV(MAJOR(gpd_dev->dev_number), MINOR(gpd_dev->dev_number));// + MAX_NUM_DOMAINS);

    /* Create a new device in order to create a /dev/ gpd node. This device
     * is a child of the PCI device.
     */
    gpd_dev->gpd_device = device_create(dev_class, &pdev->dev,
                                        dev, gpd_dev,
                                        "gpd%d/gpd", gpd_dev->id);

    return 0;
}


int device_alloc_dma_coherent(struct gpd_dev *gpd_dev){

    for (i = 0; i < NUM_DIR_QS; i++) {
        // dma alloc limit seems to be 4MB
        q_heads[i].base = dma_alloc_coherent(&gpd_dev->pdev->dev,
                                             PAGE_SIZE,
                                             &q_heads[i].dma_base,
                                             GFP_KERNEL);

        if (!q_heads[i].base) {
            printk(KERN_ERR "Unable to allocate coherent DMA to queue %d\n", i);
            if (q_heads[i].dma_base)
                dma_free_coherent(&gpd_dev->pdev->dev,
                                  PAGE_SIZE,
                                  q_heads[i].base,
                                  q_heads[i].dma_base);
            return -ENOMEM;
        } else {
            printk(KERN_NOTICE "Allocate coherent DMA to queue %d\n", i);
            printk(KERN_NOTICE "GPD: Queue PA: 0x%llx\n", virt_to_phys(q_heads[i].base));
            printk(KERN_NOTICE "GPD: Queue IOVA: 0x%llx\n", q_heads[i].dma_base);
        }
    }

    return 0;
}


int device_sriov_configure(struct pci_dev *pdev, int num_vfs) {
    if (num_vfs)
        return device_sriov_enable(pdev, num_vfs);
    else
        return device_sriov_disable(pdev, num_vfs);
}


void device_remove(struct pci_dev *pdev) {

    struct gpd_dev *gpd_dev;
    gpd_dev = pci_get_drvdata(pdev);

    pci_free_irq_vectors(pdev);
    GPD_LOG("Released IRQ vectors");

    pci_disable_msix(pdev);
    GPD_LOG("Released MSI-X vectors");

    pci_iounmap(pdev, gpd_dev->hw.csr_kva);
    pci_iounmap(pdev, gpd_dev->hw.func_kva);
    GPD_LOG("Unmapped BARs");

    pci_disable_pcie_error_reporting(pdev);
    GPD_LOG("Disabled AER");

    for(i = 0; i < NUM_DIR_QS; i++)
        dma_free_coherent(&gpd_dev->pdev->dev, PAGE_SIZE, q_heads[i].base, q_heads[i].dma_base);
    GPD_LOG("Released DMA coherent");

    pci_release_regions(pdev);
    GPD_LOG("Released MMIO/IOP regions");

    pci_clear_master(pdev);
    GPD_LOG("Disabled DMA");

    pci_disable_device(pdev);
    GPD_LOG("Disabled device");

    // FIXME: dev_class not being deleted on driver remove
    device_destroy(dev_class, MKDEV(MAJOR(gpd_dev->dev_number),
                 MINOR(gpd_dev->dev_number)));// + MAX_NUM_DOMAINS));
    class_destroy(dev_class);
    GPD_LOG("Destroyed dev_class");

    cdev_del(&gpd_dev->cdev);
    GPD_LOG("Deleted char dev");

    unregister_chrdev_region(gpd_dev_num, 1);
    GPD_LOG("Unregister Major Minor number");
}
