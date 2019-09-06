#ifndef GPD_IOCTL_H
#define GPD_IOCTL_H

#include "gpd_util.h"

// IOCTL commands
// NOTE: Export below to user program headers when using IOCTL callbacks
#define SET_Q_SIZE      _IOW('g', 'a', int*)
#define ALLOC_Q_MEM     _IO('g', 'b')
#define DESTROY_Q_MEM   _IO('g', 'c')
#define GET_Q_HEAD      _IOWR('g', 'd', long*)

// For storing virtual and physical address of queue head pointer
struct q_head {
    void *base;
    unsigned long dma_base;
};

// Temp variable to store DMA address
extern dma_addr_t dma_base;

// Queue size (default: 128)
extern int q_size;

// Array for storing all queue addresses
extern struct q_head q_heads[NUM_DIR_QS];

// Pop counter address
extern struct q_head pc_head;

/**
 * Allocates DMA coherent memory for the queues
 * @return  0 if successful
 */
int alloc_queue_mem(void);

/**
 * Destroys allocated queue memory
 */
void destory_queue_mem(void);

/**
 * Copies queue head address to user variable for a given QID
 * @param  arg queue ID
 * @return     0 if successful
 */
int get_queue_head(unsigned long arg);

#endif
