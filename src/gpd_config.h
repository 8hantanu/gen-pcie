#ifndef GPD_CONFIG_H
#define GPD_CONFIG_H

#define VENDOR_ID                   0x8086
#define DEVICE_ID                   0x270b

#define MAX_NUM_VFS                 16
#define MAX_NUM_DOMAINS             32

#define NUM_DIR_QS                  128
#define NUM_Q_ITEMS                 1024
#define ITEM_SIZE                   32

#define NUM_FUNCS_PER_DEV           (MAX_NUM_VFS + 1)
#define NUM_DEV_FILES_PER_DEVICE    (MAX_NUM_DOMAINS + 1)
#define MAX_NUM_DEV_FILES           (NUM_FUNCS_PER_DEV * NUM_DEV_FILES_PER_DEVICE)

#endif
