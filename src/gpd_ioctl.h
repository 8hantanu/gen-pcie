#ifndef GPD_IOCTL_H
#define GPD_IOCTL_H

#include "gpd_util.h"

#define SET_Q_SIZE      _IOW('g', 'a', int*)
#define ALLOC_Q_MEM     _IO('g', 'b')
#define DESTROY_Q_MEM   _IO('g', 'c')
#define GET_Q_HEAD      _IOWR('g', 'd', long*)

struct q_head {
	void *base;
	unsigned long dma_base;
};

extern dma_addr_t dma_base;
extern int q_size;
extern struct q_head q_heads[NUM_DIR_QS];
extern struct q_head pc_head;

int alloc_queue_mem(void);
void destory_queue_mem(void);
int get_queue_head(unsigned long arg);

#endif
