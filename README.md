# gen-pcie

Generic PCIe device driver

The GPD(generic PCIe driver) enables all the basic capabilities of any PCIe supported device.

**Note:** Define the `VENDOR_ID` and `DEVICE_ID` of the device to be enabled in `gpd_config.h` file.

## Build

To build the driver related file do:

```bash
$ make
```

## Load

To load the GPD dricer do:

```bash
$ insmod gpd.ko
```


## Debug

To view the capabilities enabled by the device driver do:

```bash
$ lspci -d <VENDOR_ID:DEVICE_ID> -vvv
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
