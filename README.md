# gen-pcie

The GPD(generic PCIe driver) enables all the basic capabilities of any PCIe supported device.


## Prereqs

Make sure the kernel sources are same while building and testing.

```bash
$ export KSRC=/tools/AGRtools/fedora24/4.8.6-201.frio.fc24.x86_64
```


## Build

To build the driver related file do:

```bash
$ make
```


## Load

To load the GPD driver do:

```bash
$ insmod gpd.ko
```

## Debug

To view device capabilities and status do:

```bash
$ lspci -d [VENDOR_ID:DEVICE_ID] -vvv
```

To view kernel logs specific to GPD do:

```bash
$ dmesg | grep GPD
```

To print all the kernel logs to console do:

```bash
$ echo 8 > /proc/sys/kernel/printk
```


## Remove

To unload the driver do:

```bash
$ rmmod gpd
```

To clean all kernel objects files do:

```bash
$ make clean
```


## Extras

To save logs to a file do:

```bash
$ lspci -d [VENDOR_ID:DEVICE_ID] -vvv > [file_name]
```

To compare two files do:

```bash
$ diff [file_1] [file_2]
```


## Config

The `DEVICE_ID`, `VENDOR_ID`and some other parameters can be defined in the `gpd_config.h` file.
