#ifndef GPD_SRIOV_H
#define GPD_SRIOV_H

#include "gpd_util.h"

#define GPD_VF_SYSFS_RD_FUNC(id) 								\
static ssize_t gpd_sysfs_vf ## id ## _read(			      		\
	struct device *dev,				      						\
	struct device_attribute *attr,			      				\
	char *buf)					      							\
{							      								\
	GPD_LOG("VF_SYSFS_READ called");							\
	return sprintf(buf, "%d", param_0);							\
}

#define GPD_VF_SYSFS_WR_FUNC(id) 								\
static ssize_t gpd_sysfs_vf ## id ## _write(		    	  	\
	struct device *dev,				      						\
	struct device_attribute *attr,			      				\
	const char *buf,				      						\
	size_t count)					      						\
{							      								\
	GPD_LOG("VF_SYSFS_WRITE called");							\
	return count;										 		\
}

#define GPD_VF_DEVICE_ATTRS(id) 								\
static struct device_attribute			    					\
dev_attr_vf ## id ## _param_0 =		    						\
	__ATTR(param_0,		    									\
	       0644,				    							\
	       gpd_sysfs_vf ## id ## _read,	    					\
	       gpd_sysfs_vf ## id ## _write);	   	 				\
static struct device_attribute			    					\
dev_attr_vf ## id ## _param_1 =		    						\
	__ATTR(param_1,		    									\
   	       0644,				    							\
	       gpd_sysfs_vf ## id ## _read,	    					\
	       gpd_sysfs_vf ## id ## _write);	    				\
static struct device_attribute			    					\
dev_attr_vf ## id ## _param_2 =		    						\
	__ATTR(param_2,		    									\
  	       0644,				    							\
	       gpd_sysfs_vf ## id ## _read,	    					\
	       gpd_sysfs_vf ## id ## _write);

#define GPD_VF_SYSFS_ATTRS(id) 									\
GPD_VF_DEVICE_ATTRS(id);										\
static struct attribute *gpd_vf ## id ## _attrs[] = {			\
	&dev_attr_vf ## id ## _param_0.attr,						\
	&dev_attr_vf ## id ## _param_1.attr,						\
	&dev_attr_vf ## id ## _param_2.attr,						\
	NULL														\
}

#define GPD_VF_SYSFS_ATTR_GROUP(id)                             \
GPD_VF_SYSFS_ATTRS(id);                                         \
static struct attribute_group gpd_vf ## id ## _attr_group = {   \
    .attrs = gpd_vf ## id ## _attrs,                            \
    .name = "vf" #id "_resources"                               \
}

volatile int param_0 = 0;
volatile int param_1 = 0;
volatile int param_2 = 0;

GPD_VF_SYSFS_RD_FUNC(0);
GPD_VF_SYSFS_RD_FUNC(1);
GPD_VF_SYSFS_RD_FUNC(2);
GPD_VF_SYSFS_RD_FUNC(3);
GPD_VF_SYSFS_RD_FUNC(4);
GPD_VF_SYSFS_RD_FUNC(5);
GPD_VF_SYSFS_RD_FUNC(6);
GPD_VF_SYSFS_RD_FUNC(7);
GPD_VF_SYSFS_RD_FUNC(8);
GPD_VF_SYSFS_RD_FUNC(9);
GPD_VF_SYSFS_RD_FUNC(10);
GPD_VF_SYSFS_RD_FUNC(11);
GPD_VF_SYSFS_RD_FUNC(12);
GPD_VF_SYSFS_RD_FUNC(13);
GPD_VF_SYSFS_RD_FUNC(14);
GPD_VF_SYSFS_RD_FUNC(15);

GPD_VF_SYSFS_WR_FUNC(0);
GPD_VF_SYSFS_WR_FUNC(1);
GPD_VF_SYSFS_WR_FUNC(2);
GPD_VF_SYSFS_WR_FUNC(3);
GPD_VF_SYSFS_WR_FUNC(4);
GPD_VF_SYSFS_WR_FUNC(5);
GPD_VF_SYSFS_WR_FUNC(6);
GPD_VF_SYSFS_WR_FUNC(7);
GPD_VF_SYSFS_WR_FUNC(8);
GPD_VF_SYSFS_WR_FUNC(9);
GPD_VF_SYSFS_WR_FUNC(10);
GPD_VF_SYSFS_WR_FUNC(11);
GPD_VF_SYSFS_WR_FUNC(12);
GPD_VF_SYSFS_WR_FUNC(13);
GPD_VF_SYSFS_WR_FUNC(14);
GPD_VF_SYSFS_WR_FUNC(15);

GPD_VF_SYSFS_ATTR_GROUP(0);
GPD_VF_SYSFS_ATTR_GROUP(1);
GPD_VF_SYSFS_ATTR_GROUP(2);
GPD_VF_SYSFS_ATTR_GROUP(3);
GPD_VF_SYSFS_ATTR_GROUP(4);
GPD_VF_SYSFS_ATTR_GROUP(5);
GPD_VF_SYSFS_ATTR_GROUP(6);
GPD_VF_SYSFS_ATTR_GROUP(7);
GPD_VF_SYSFS_ATTR_GROUP(8);
GPD_VF_SYSFS_ATTR_GROUP(9);
GPD_VF_SYSFS_ATTR_GROUP(10);
GPD_VF_SYSFS_ATTR_GROUP(11);
GPD_VF_SYSFS_ATTR_GROUP(12);
GPD_VF_SYSFS_ATTR_GROUP(13);
GPD_VF_SYSFS_ATTR_GROUP(14);
GPD_VF_SYSFS_ATTR_GROUP(15);


const struct attribute_group *gpd_vf_attrs[] = {
	&gpd_vf0_attr_group,
    &gpd_vf1_attr_group,
    &gpd_vf2_attr_group,
    &gpd_vf3_attr_group,
	&gpd_vf4_attr_group,
    &gpd_vf5_attr_group,
    &gpd_vf6_attr_group,
    &gpd_vf7_attr_group,
	&gpd_vf8_attr_group,
    &gpd_vf9_attr_group,
    &gpd_vf10_attr_group,
    &gpd_vf11_attr_group,
	&gpd_vf12_attr_group,
    &gpd_vf13_attr_group,
    &gpd_vf14_attr_group,
    &gpd_vf15_attr_group,
};


int dev_sriov_enable(struct pci_dev *pdev, int num_vfs) {

    int ret, i;

    ret = pci_enable_sriov(pdev, num_vfs);
    if (ret) {
        GPD_ERR("Enable SR-IOV failed");
        return ret;
    } else {
        GPD_LOG("SR-IOV enabled");
    }

    for (i = 0; i < num_vfs; i++) {
        ret = sysfs_create_group(&pdev->dev.kobj, gpd_vf_attrs[i]);
        if (ret) {
            GPD_ERR("Failed to create VF sysfs attr groups");
            pci_disable_sriov(pdev);
            return ret;
        }
    }

    return num_vfs;
}


int dev_sriov_disable(struct pci_dev *pdev, int num_vfs) {

    int i;

    for (i = 0; i < pci_num_vf(pdev); i++)
        sysfs_remove_group(&pdev->dev.kobj, gpd_vf_attrs[i]);

    pci_disable_sriov(pdev);

    return 0;
}

#endif
