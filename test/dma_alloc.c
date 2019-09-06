#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SET_Q_SIZE      _IOW('g', 'a', int*)
#define ALLOC_Q_MEM     _IO('g', 'b')
#define DESTROY_Q_MEM   _IO('g', 'c')
#define GET_Q_HEAD      _IOWR('g', 'd', long*)

/**
 * User program for testing DMA allocations through IOCTL calls
 *
 * @param  argc Number of input args (must be 3 if not default values assigned)
 * @param  argv 1st: ./dma_alloc
 *              2nd: number of queues(if argv[2] = 1) or qid (if argv[2] = 0)
 *              3rd: set 1 to allocate queue mem
 *                   set 0 to get head of qid or pc
 *
 * @return 0 if successful
 *
 * @details Usage:
 *             To allocate memory for n queues do:
 *                 $ ./dma_alloc <n> 1
 *             To deallocate memory, send 0 while allocation:
 *                 $ ./dma_alloc 0 1
 *             To get queue head qid i do:
 *                 $ ./dma_alloc <i> 0
 *             To get pop counter head, send qsize in qid:
 *                 $ ./dma_alloc <qsize> 0
 *          Examples:
 *             To allocate memory for 8 queues do:
 *                 $ ./dma_alloc 8 1
 *             To get queue head qid 6 do:
 *                 $ ./dma_alloc 6 0
 *             To get pop counter head, send qsize in qid:
 *                 $ ./dma_alloc 8 0
 */
int main(int argc, char *argv[]) {

    int fd, i, allocate;
    unsigned long qid_addr[128], size;
    char *ptr;

    fd = open("/dev/gpd", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file\n");
        return -1;
    }

    if (argc == 3) {
        size = strtol(argv[1], &ptr, 10);
        allocate = strtol(argv[2], &ptr, 10);
    } else {
        size = 128;
        allocate = 1;
    }

    if (allocate) {

        if (size) {

            printf("Number of queues set to %lx\n", size);

            printf("Sending queue size to GPD\n");
            ioctl(fd, SET_Q_SIZE, (int*) &size);

            printf("Allocating queue memory\n");
            ioctl(fd, ALLOC_Q_MEM);

            // for(i = 0; i < size; i++) {
            //     qid_addr[i] = i;
            //     ioctl(fd, GET_Q_HEAD, (long*) &qid_addr[i]);
            //     printf("QID %d head pointer: 0x%lx\n", i, qid_addr[i]);
            // }

        } else {

            printf("Unallocating queue memory\n");
            ioctl(fd, DESTROY_Q_MEM);

        }

    } else {
        ioctl(fd, GET_Q_HEAD, (long*) &size);
        printf("0x%lx", size);
    }

    close(fd);

    return 0;
}
