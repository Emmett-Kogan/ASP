#include <linux/module.h>	/* for modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev utilities */
#include <linux/errno.h>	/* error codes */
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/semaphore.h>

#define MYDEV_NAME "mycdrv"
#define RAMDISK_SIZE (size_t) (16 * PAGE_SIZE) // ramdisk size 
#define CDRV_IOC_MAGIC 'Z'
#define ASP_CLEAR_BUF _IO(CDRV_IOC_MAGIC, 1)
#define ERR(lval, rval, label) { lval = rval; goto label; }   

struct ASP_mycdrv {
	struct cdev cdev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
    size_t ramdisk_size;
};

// Pointer to AoS for all device driver structs
static struct ASP_mycdrv *devices;

// Node class for making nodes
static struct class *node_class;

// Parameters
static int majorno = 500, minorno = 0;
static size_t size = RAMDISK_SIZE;
static int numdevices = 3;

module_param(majorno, int, S_IRUGO);
module_param(numdevices, int, S_IRUGO);
module_param(size, long, S_IRUGO);

/* should update the file position pointer
 * based on the offset requested. You should set the file position pointer as re-
 * quested as long as it does not go out of bounds, i.e., 0 ≤ requestedposition.
 * In the case of a request that goes beyond end of the buffer, your imple-
 * mentation needs to expand the buffer and fill the new region with zeros.
 */
static loff_t mycdrv_llseek(struct file *fptr, loff_t off, int whence)
{
    struct ASP_mycdrv *dev = fptr->private_data;
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    // Calculate new offset value
    loff_t newpos;
    switch (whence) {
    case SEEK_SET:
        newpos = off;
        break;
    case SEEK_CUR:
        newpos = fptr->f_pos + off;
        break;
    case SEEK_END:
        newpos = dev->ramdisk_size + off;
        break;
    default:
        ERR(newpos, -EINVAL, exit);
    }
    
    // Validate newpos
    if (newpos < 0)
        ERR(newpos, -EDOM, exit);

    // Check if we need to expand buffer
    if (newpos > dev->ramdisk_size) {
        // New region should be filled with zeros (for some dumbass reason)
        char *tmp = (char *) kcalloc(newpos, 1, GFP_KERNEL);       // new ramdisk buffer
        memcpy(tmp, dev->ramdisk, size);
        kfree(dev->ramdisk);
        dev->ramdisk = tmp;
    }

    fptr->f_pos = newpos;
    pr_info("SEEK: pos=%lld\n", fptr->f_pos);

exit:
    up(&dev->sem);
    return newpos;
}

static ssize_t mycdrv_read(struct file *fptr, char __user *buf, size_t lbuf, loff_t *ppos)
{   
    struct ASP_mycdrv *dev = fptr->private_data;
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    ssize_t nbytes;
    if ((lbuf + *ppos) > dev->ramdisk_size) {
        pr_info("Error: attempted to read past end of device\n");
        ERR(nbytes, 0, exit);
    }

    nbytes = lbuf - copy_to_user(buf, dev->ramdisk+*ppos, lbuf);
    *ppos += nbytes;
    pr_info("READ: nbytes=%ld, pos=%lld\n", nbytes, *ppos);

exit:
    up(&dev->sem);
    return nbytes;
}

static ssize_t mycdrv_write(struct file *fptr, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    struct ASP_mycdrv *dev = fptr->private_data;
    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    ssize_t nbytes;
    if ((lbuf + *ppos) > dev->ramdisk_size) {
        pr_info("Error: attempted to write past end of device\n");
        ERR(nbytes, 0, exit);
    }

    nbytes = lbuf - copy_from_user(dev->ramdisk + *ppos, buf, lbuf);
    *ppos += nbytes;
    pr_info("WRITE: nbytes=%ld, pos=%lld\n", nbytes, *ppos);
    
exit:
    up(&dev->sem);
    return nbytes;
}

static long mycdrv_ioctl(struct file *fptr, unsigned int cmd, unsigned long arg) 
{
    // Verify cmd
    if (_IOC_TYPE(cmd) != CDRV_IOC_MAGIC) 
        return -ENOTTY;
    
    // Start critical section
    struct ASP_mycdrv *dev = fptr->private_data;
    if(down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    long retval = 0;
    switch (cmd) {
    case ASP_CLEAR_BUF:
        // Clear the ramdisk and set position to 0
        memset(dev->ramdisk, 0, dev->ramdisk_size);
        fptr->f_pos = 0;
        break;
    default:
        ERR(retval, -ENOTTY, exit);
    }

exit:
    up(&dev->sem);
    return retval;
}

static int mycdrv_open(struct inode *inode, struct file *fptr) 
{
    struct ASP_mycdrv *dev = container_of(inode->i_cdev, struct ASP_mycdrv, cdev);
    fptr->private_data = dev;
    pr_info("OPEN: %s%d:\n", MYDEV_NAME, dev->devNo);
    return 0;
}

static int mycdrv_release(struct inode *inode, struct file *fptr) 
{
    struct ASP_mycdrv *dev = fptr->private_data;
    pr_info("RELEASE: %s%d:\n\n", MYDEV_NAME, dev->devNo);
    return 0;
}

static const struct file_operations mycdrv_fops = {
    .owner = THIS_MODULE,
    .llseek = mycdrv_llseek,
    .read = mycdrv_read,
    .write = mycdrv_write,
    .unlocked_ioctl = mycdrv_ioctl,
    .open = mycdrv_open,
    .release = mycdrv_release,
};

static int __init my_init(void) 
{
    // Allocate space for each device driver struct
    devices = (struct ASP_mycdrv *) kcalloc(sizeof(struct ASP_mycdrv), numdevices, GFP_KERNEL);

    // Reserve device numbers
    register_chrdev_region(MKDEV(majorno, minorno), numdevices, MYDEV_NAME);

    // Make class for device nodes
    node_class = class_create("ASP_mycdrv");

    // Initialize each device
    for (int i = 0; i < numdevices; i++) {
        // Initialize device struct's buffer, minor number and sememaphore
        devices[i].ramdisk = (char *) kcalloc(size, 1, GFP_KERNEL);
        devices[i].ramdisk_size = size;
        devices[i].devNo = minorno+i;
        sema_init(&devices[i].sem, 1);

        // Initialize nested cdev struct and inform kernel
        cdev_init(&devices[i].cdev, &mycdrv_fops);
        cdev_add(&devices[i].cdev, MKDEV(majorno, minorno+i), 1);

        // Create device node
        device_create(node_class, NULL, MKDEV(majorno, minorno+i), NULL, "mycdrv%d", minorno+i);
    }

    // Log success and return
    pr_info("INIT: Succeeded in registering %d %ss\n\n", numdevices, MYDEV_NAME);
    return 0;
}

static void __exit my_exit(void) 
{
    // For each device:
    for (int i = 0; i < numdevices; i++) {
        // delete node, delete device, free ramdisk
        device_destroy(node_class, MKDEV(majorno, minorno+i));
        cdev_del(&devices[i].cdev);
        kfree(devices[i].ramdisk);
    }
 
    // need to delete the device class as well as unregister the major/minor numbers
    class_destroy(node_class);
    unregister_chrdev_region(MKDEV(majorno,minorno), numdevices);

    // free devices structs
    kfree(devices);

    pr_info("EXIT: Succeeded in unregistering %d %ss\n", numdevices, MYDEV_NAME);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("user");
MODULE_LICENSE("GPL v2");
