# gen-pcie

The GPD(generic PCIe driver) enables all the basic capabilities of any PCIe supported device.

## Prereqs

Make sure the kernel sources are same while building and testing.

```bash
$ export KSRC=/tools/AGRtools/fedora24/4.8.6-201.frio.fc24.x86_64
```


Define the `VENDOR_ID` and `DEVICE_ID` of the device to be enabled in `gpd_config.h` file.

**Note:** The following code has been tested on `8086:270b` device on the `FRIO-071B`system using `/tools/AGRtools/fedora24/4.8.6-201.frio.fc24.x86_64` as the kernel source.


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

**Note**: In our case VENDOR_ID is 8086, DEVICE_ID is 270b

To view kernel logs specific to GPD do:

```bash
$ dmesg | grep GPD
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

- To save logs to a file do:
```bash
$ lspci -d [VENDOR_ID:DEVICE_ID] -vvv > [file_name]
```

- To compare two files do:
```bash
$ diff [file_1] [file_2]
```
