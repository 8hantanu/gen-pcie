# gen-pcie

Generic PCIe device driver

The GPD(generic PCIe driver) enables all the basic capabilities of any PCIe supported device.

**Note:** Define the `VENDOR_ID` and `DEVICE_ID` of the device to be enabled in `gpd_main.h` file.

## Build

To build the driver related file do:

```bash
$ make
```

## Load

To load the GPD dricer do:

```bash
$ insmod gpd_main.ko
```


## Debug

To view the capabilities enabled by the device driver do:

```bash
$ lspci -d <VENDOR_ID:DEVICE_ID> -vvv
```

To view kernel logs specific to GPD do:

```bash
$ dmesg | grep GPD
```

## Remove

To unload the driver do:

```bash
$ rmmod gpd_main
```
