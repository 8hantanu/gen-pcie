#include "gpd_fops.h"

ssize_t gpd_write(struct file *f,
			 const char __user *buf,
			 size_t len,
			 loff_t *offset)
{
	struct gpd_dev *dev;

	dev = container_of(f->f_inode->i_cdev, struct gpd_dev, cdev);

	return 0;
}
