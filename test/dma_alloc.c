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

unsigned long main(int argc, char *argv[]) {

    int fd, i, allocate;
    unsigned long qid_addr[128], size;
    char *ptr;

    fd = open("/dev/gpd", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file\n");
        return -1;
    }

    if (argc == 3)  {
        size = strtol(argv[1], &ptr, 10);
        allocate = strtol(argv[2], &ptr, 10);
    } else {
        size = 128;
        allocate = 1;
    }

    if (allocate) {

        if (size) {

            printf("Number of queues set to %d\n", size);

            printf("Sending queue size to GPD\n");
            ioctl(fd, SET_Q_SIZE, (int*) &size);

            printf("Allocating queue memory\n");
            ioctl(fd, ALLOC_Q_MEM);

            for(i = 0; i < size; i++) {
                qid_addr[i] = i;
                ioctl(fd, GET_Q_HEAD, (long*) &qid_addr[i]);
                printf("QID %d head pointer: 0x%lx\n", i, qid_addr[i]);
            }

        } else {

            printf("Unallocating queue memory\n");
            ioctl(fd, DESTROY_Q_MEM);

        }

    } else {
        printf("QID %d pointer:\n", size);
        ioctl(fd, GET_Q_HEAD, (long*) &size);
        printf("0x%lx\n", size);
    }

    printf("Closing driver\n");
    close(fd);

    return size;
}
