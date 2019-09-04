#include "gpd_dev.h"
#include "gpd_sriov.h"

struct pci_dev *pci_dev;
struct msix_entry *entries;

struct file_operations gpd_fops = {
    .owner   = THIS_MODULE,
//  .open    = gpd_open,
//  .release = gpd_close,
//  .read    = gpd_read,
//  .write   = gpd_write,
//  .mmap    = gpd_mmap,
#if KERNEL_VERSION(2, 6, 35) <= LINUX_VERSION_CODE
    .unlocked_ioctl = gpd_ioctl,
#else
    .ioctl   = gpd_ioctl,
#endif
};

dev_t gpd_dev_num;
struct class *dev_class;

int dev_probe(struct pci_dev *pdev, const struct pci_device_id *id) {

    struct gpd_dev *gpd_dev;
    gpd_dev = devm_kcalloc(&pdev->dev, 1, sizeof(struct gpd_dev), GFP_KERNEL);
    if (!gpd_dev)
        return -ENOMEM;

    pci_set_drvdata(pdev, gpd_dev);

    gpd_dev->pdev = pdev;

    if (dev_function_reset(pdev, true))
        GPD_ERR("Function reset failed");
    else
        GPD_LOG("Device Reset");

    dev_init(gpd_dev);

    return 0;
}


int dev_init(struct gpd_dev *gpd_dev) {

    if (pci_enable_device(gpd_dev->pdev) != 0 )
        GPD_ERR("Failed while enabling");

    // VFs
    // dev_sriov_configure(gpd_dev->pdev, 2);

    // Reserve access to pdev by gpd
    if (pci_request_regions(gpd_dev->pdev, gpd_name))
        GPD_ERR("MMIO/IOP map failed");
    else
        GPD_LOG("Mapped MMIO/IOP regions");

    // Enable bus mastering
    pci_set_master(gpd_dev->pdev);
    GPD_LOG("Enabled bus mastering");

    // Enable PCI INTx
    pci_intx(gpd_dev->pdev, 0);

    // Enable AER capability
    if (pci_enable_pcie_error_reporting(gpd_dev->pdev))
        GPD_ERR("AER enable failed");
    else
        GPD_LOG("Enabled AER");

    // Set DMA Mask
    dma_set_mask_and_coherent(&gpd_dev->pdev->dev, DMA_BIT_MASK(64));

    // Enable interrupt vectors
    // int vecs_available = pci_msix_vec_count(gpd_dev->pdev);
    // int vecs_assigned = pci_enable_msix_range(gpd_dev->pdev, entries, 1, vecs_available);
    // if (vecs_available != vecs_assigned)
    //     GPD_ERR("MSI-X vectors enable failed");
    // else
    //     GPD_LOG("Enabled MSI-X vectors");
    // printk(KERN_NOTICE "GPD: Total %d MSI vecs available", vecs_available);
    // printk(KERN_NOTICE "GPD: Total %d MSI vecs assigned", vecs_assigned);
    //
    // pci_alloc_irq_vectors(gpd_dev->pdev, 9, 9, PCI_IRQ_MSIX);

    // UEMsk
    dev_mask_uerr(gpd_dev->pdev);

    // BAR iomap
    dev_map_bar_space(gpd_dev);

    // Add cdev
    dev_add_cdev(gpd_dev, gpd_dev_num, &gpd_fops);

    // Create cdev
    dev_pf_create(gpd_dev, dev_class);

    // Set device power state to D3
    // pci_set_power_state(gpd_dev->pdev, PCI_D3hot);

    // Allocate DMA coherent
    // dev_alloc_dma_coherent(gpd_dev);

    // Cleans up uncorrectable error status registers
    pci_cleanup_aer_uncorrect_error_status(gpd_dev->pdev);

    return 0;
}


int dev_map_bar_space(struct gpd_dev *gpd_dev) {

    gpd_dev->hw.csr_kva = pci_iomap(gpd_dev->pdev, 0, 0);
    gpd_dev->hw.csr_phys_addr = pci_resource_start(gpd_dev->pdev, 0);
    if (!gpd_dev->hw.csr_kva) {
        GPD_ERR("Failed to map BAR 0");
        // TODO: Add fail flow
    } else {
        GPD_LOG("Mapped BAR 0 space");
        printk(KERN_NOTICE "GPD: BAR 0 start at 0x%llx\n", pci_resource_start(gpd_dev->pdev, 0));
        printk(KERN_NOTICE "GPD: BAR 0 len is %llu\n", pci_resource_len(gpd_dev->pdev, 0));
    }
    gpd_dev->hw.func_kva = pci_iomap(gpd_dev->pdev, 2, 0);
    gpd_dev->hw.func_phys_addr = pci_resource_start(gpd_dev->pdev, 2);
    if (!gpd_dev->hw.func_kva) {
        GPD_ERR("Failed to map BAR 2");
        // TODO: Add fail flow
    } else {
        GPD_LOG("Mapped BAR 2 space");
        printk(KERN_NOTICE "GPD: BAR 2 start at 0x%llx\n", pci_resource_start(gpd_dev->pdev, 2));
        printk(KERN_NOTICE "GPD: BAR 2 len is %llu\n", pci_resource_len(gpd_dev->pdev, 2));
    }

    return 0;
}


int dev_add_cdev(struct gpd_dev *gpd_dev,
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


int dev_pf_create(struct gpd_dev *gpd_dev,
                  struct class *dev_class)
{
    dev_t dev;

    dev = MKDEV(MAJOR(gpd_dev->dev_number), MINOR(gpd_dev->dev_number));// + MAX_NUM_DOMAINS);

    /* Create a new device in order to create a /dev/ gpd node. This device
     * is a child of the PCI device.
     */
    gpd_dev->gpd_device = device_create(dev_class, &gpd_dev->pdev->dev, dev, gpd_dev, "gpd");

    return 0;
}


void dev_mask_uerr(struct pci_dev *pdev) {

    u32 mask;
    int pos = pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ERR);

    pci_read_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, &mask);
    mask |= PCI_ERR_UNC_UNSUP;
    pci_write_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, mask);
}


int dev_function_reset(struct pci_dev *pdev, bool save_state) {

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
    // TODO: Check kernel version
    ret = __pci_reset_function_locked(pdev);
    if (ret)
        return ret;

    if (save_state)
        pci_restore_state(pdev);

    return 0;
}


// NOTE: Allocation now done during runtime through IOCTL calls
// int dev_alloc_dma_coherent(struct gpd_dev *gpd_dev){
//
//     for (i = 0; i < NUM_DIR_QS; i++) {
//
//         dma_base = 0;
//         // dma alloc limit seems to be 4MB
//         q_heads[i].base = dma_alloc_coherent(&gpd_dev->pdev->dev,
//                                              PAGE_SIZE,
//                                              &dma_base,
//                                              GFP_KERNEL);
//
//         q_heads[i].dma_base = (int) dma_base;
//
//         if (!q_heads[i].base) {
//             printk(KERN_ERR "Unable to allocate coherent DMA to queue %d\n", i);
//             if (dma_base)
//                 dma_free_coherent(&gpd_dev->pdev->dev,
//                                   PAGE_SIZE,
//                                   q_heads[i].base,
//                                   dma_base);
//             return -ENOMEM;
//         } else {
//             printk(KERN_NOTICE "GPD: Allocate coherent DMA to queue %d\n", i);
//             printk(KERN_NOTICE "GPD: Queue PA: 0x%llx\n", virt_to_phys(q_heads[i].base));
//             printk(KERN_NOTICE "GPD: Queue IOVA: 0x%llx\n", q_heads[i].dma_base);
//         }
//     }
//
//     return 0;
// }


int dev_sriov_configure(struct pci_dev *pdev, int num_vfs) {
    if (num_vfs)
        return dev_sriov_enable(pdev, num_vfs);
    else
        return dev_sriov_disable(pdev, num_vfs);
}


void dev_remove(struct pci_dev *pdev) {

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

    // for(i = 0; i < NUM_DIR_QS; i++)
    //     dma_free_coherent(&gpd_dev->pdev->dev, PAGE_SIZE, q_heads[i].base, q_heads[i].dma_base);
    // GPD_LOG("Released DMA coherent");

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
