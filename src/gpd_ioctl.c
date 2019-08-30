#include "gpd_ioctl.h"

int i;

dma_addr_t dma_base;
int q_size = NUM_DIR_QS;
struct q_head q_heads[NUM_DIR_QS];


int alloc_queue_mem(void) {

    for (i = 0; i < q_size; i++) {
        dma_base = 0;
        // dma alloc limit seems to be 4MB
        q_heads[i].base = dma_alloc_coherent(NULL, PAGE_SIZE, &dma_base, GFP_KERNEL);
        q_heads[i].dma_base = dma_base;

        if (!q_heads[i].base) {
            printk(KERN_ERR "Unable to allocate coherent DMA to queue %d\n", i);
            if (dma_base)
                dma_free_coherent(NULL, PAGE_SIZE, q_heads[i].base, dma_base);
            return -ENOMEM;
        } else {
            printk(KERN_NOTICE "GPD: Allocate coherent DMA to queue %d\n", i);
            printk(KERN_NOTICE "GPD: Queue PA: 0x%llx\n", virt_to_phys(q_heads[i].base));
            printk(KERN_NOTICE "GPD: Queue IOVA: 0x%lx\n", q_heads[i].dma_base);
        }
    }

    return 0;
}


void destory_queue_mem(void) {

    for(i = 0; i < q_size; i++)
        dma_free_coherent(NULL, PAGE_SIZE, q_heads[i].base, q_heads[i].dma_base);

    GPD_LOG("Released DMA coherent");
}


int get_queue_head(long* arg) {

    int qid;
    if (copy_from_user(&qid ,(long*) arg, sizeof(qid))) {
        GPD_ERR("Failed to get QID");
        return 1; // TODO: change return code
    }

    if (qid < q_size){
        if (copy_to_user((long*) arg, &q_heads[i].dma_base, sizeof(q_heads[i].dma_base)))
            GPD_ERR("Failed to send queue head pointer");
        else
            printk(KERN_NOTICE "GPD: Queue head pointer for QID %d is 0x%lx\n", qid, *arg);
    } else {
        printk(KERN_ERR "GPD: Invalid QID %d. QID must be less than %d\n", qid, q_size);
        return 1; // TODO: change return code
    }

    return 0;
}
