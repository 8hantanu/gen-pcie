#include "gpd_fops.h"

long qid;
unsigned long q_addr;

int gpd_open(struct inode *i, struct file *f) {
	GPD_LOG("Device file opened");
	return 0;
}

// ssize_t gpd_write(struct file *f, const char __user *buf, size_t len, loff_t *offset) {
// 	struct gpd_dev *dev;
//
// 	dev = container_of(f->f_inode->i_cdev, struct gpd_dev, cdev);
//
// 	return 0;
// }

#if KERNEL_VERSION(2, 6, 35) <= LINUX_VERSION_CODE
  long gpd_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#else
  int gpd_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	switch(cmd) {
	    case SET_Q_SIZE:
            if (copy_from_user(&q_size ,(int*) arg, sizeof(q_size)))
				GPD_ERR("Failed to get number of queues");
			else
				printk(KERN_NOTICE "GPD: Setting number of queues to %d\n", q_size);
            break;
	    case ALLOC_Q_MEM:
			printk(KERN_NOTICE "GPD: Allocating memory to queue of size %d\n", q_size);
            alloc_queue_mem();
			break;
	    case DESTROY_Q_MEM:
			GPD_LOG("Releasing queue memory");
            destory_queue_mem();
			break;
	    case GET_Q_HEAD:
		    if (copy_from_user(&qid, (long*) arg, sizeof(qid))) {
		        GPD_ERR("Failed to get QID");
		        return 1; // TODO: change return code
		    }

		    if (qid < q_size){
				q_addr = q_heads[qid].dma_base;
		        if (copy_to_user((long*) arg, &q_addr, sizeof(q_addr)))
		            GPD_ERR("Failed to send queue head pointer");
		        else {
		            printk(KERN_NOTICE "GPD: Queue head pointer for QID %ld is 0x%lx\n", qid, q_addr);
				}
		    } else {
		        printk(KERN_ERR "GPD: Invalid QID %ld. QID must be less than %d\n", qid, q_size);
		        return 1; // TODO: change return code
		    }
			break;
    }
	return 0;
}
