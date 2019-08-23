# gen-pcie

The GPD(generic PCIe driver) enables all the basic capabilities of any PCIe supported device.

## Prereqs

Make sure the kernel sources are same while building and testing.

```bash
$ export KSRC=/tools/AGRtools/fedora24/4.8.6-201.frio.fc24.x86_64
```

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

**Note:** If testing HQM using GPD, rename driver to _gpd_ to _hqm_. Refer **Changing driver name** section in _gen-pcie.wiki/Build.md_. 

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
