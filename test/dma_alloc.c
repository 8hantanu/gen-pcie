#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SET_Q_SIZE      _IOW('g', 'a', int*)
#define ALLOC_Q_MEM     _IO('g', 'b')
#define DESTROY_Q_MEM   _IO('g', 'c')
#define GET_Q_HEAD      _IOWR('g', 'd', long*)

int main(int argc, char *argv[]) {

    printf("GPD DMA NON_CONTIG MEM ALLOC TEST\n");

    int fd, i, size;
    long qid_addr[128];
    char *ptr;

    printf("Opening GPD driver\n");
    fd = open("/dev/gpd", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file\n");
        return 0;
    }

    if (argc == 2)
        size = strtol(argv[1], &ptr, 10);
    else
        size = 128;
    printf("Number of queues set to %d", size);

    printf("Sending queue size to GPD\n");
    ioctl(fd, SET_Q_SIZE, (int*) &size);

    printf("Allocating queue memory\n");
    ioctl(fd, ALLOC_Q_MEM);

    for(i = 0; i < size; i++) {
        qid_addr[i] = i;
        ioctl(fd, GET_Q_HEAD, (int*) &qid_addr[i]);
        printf("QID %d head pointer: 0x%ld\n", i, qid_addr[i]);
    }

    // printf("Unallocating queue memory\n");
    // ioctl(fd, DESTROY_Q_MEM);

    printf("Closing driver\n");
    close(fd);
}
