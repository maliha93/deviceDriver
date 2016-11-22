/* Prototype module for second mandatory DM510 assignment */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <linux/cdev.h>

//(1) Create a structure for our fake device
struct fake_device{
	char data[100];
	struct semaphore sem;
}virtual_device;

//(2) To later register our device we need a cdev object and some other variables
struct cdev *mcdev;	//m stands 'my'
int major_number;	//will store our major number- extracted from dev_t using macro
int ret;		//will be used to hold return values of functions;this is because the kernel stack is so small
			//so declaring variables all over the pass in our module functions eats up the stack very fast

dev_t dev_num; //will hold major number that kernel gives us
		//name--> appears in /proc/devices

#define DEVICE_NAME "sysprogdevice"

//(7)called on device_file open
//	inode reference to the file on disk
//	and contains information about that file
//	struct file represents an abstract open file
int device_open(struct inode *inode, struct file *filp){
	//only allows one process to pen this device by using a semaphore as a mutual exclusion lock
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "sysprogdevice: could not lock device during open");
		return -1;
	}

	printk(KERN_INFO "sysprogdevice: opened device\n");
	return 0;

}

//(8) called when user wants to get information from the device
ssize_t device_read(struct file* filp, char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//take data from kernel space(device) to user space (process)
	//copy_to_user (destination, source, sizeTo Taransfer)
	printk(KERN_ALERT "sysprogdevice: Reading from device\n");

	ret = copy_to_user(bufSourceData, virtual_device.data, bufCount);
	
	return ret;
}


//(9) called when user wants to send information to the device
ssize_t device_write(struct file* filep, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//send data from user to kernel
	//copy_from_user (dest, source, count)

	printk(KERN_INFO "sysprogdevice: writing to device - %lu\n", bufCount);
	ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);
	
	return ret;
}

//(10) called upon user close
int device_close(struct inode *inode, struct file *filp){
	//by calling up, which is opposite of down for semaphore, we release the mutex that we obtained on open
//this has the effect of allowing other process to use the device now
	up(&virtual_device.sem);
	printk(KERN_INFO "sysprogdevice:closed device\n");
	return 0;
}

//(6) Tell the kernel which functions to call when user operates on our device file
struct file_operations fops = {
	.owner = THIS_MODULE,		//prevent unloading of this module when operations are in use
	.open = device_open,		//points to the method to call when opening the device
	.release = device_close,	//points to the method to call when closing the device
	.write = device_write,		//points to the method to call when writing the device
	.read = device_read		//points to the method to call when reading the device
};


static int driver_entry(void){
	//(3) REgister our device with the system: a two step process
	//step(1) use dynamic allocation to assign our device
	//a major number--alloc_chrdev_region(dev_t*,uint fminor, uint count, char* name)
	ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret < 0){
		printk(KERN_ALERT "sysprogdevice: Failed to allocate a device\n");
		return ret; //propagate error
	}
	major_number = MAJOR(dev_num); //extracts the major number
	printk(KERN_INFO "sysprogdevice: Failed to allocate a device\n");
	printk(KERN_INFO "sysprogdevice: \tuse \"mknod /dev/%s c %d 0\" for device file",DEVICE_NAME,major_number); //dmesg
	//step(2)
	mcdev= cdev_alloc(); //create our cdev structure, initialized our cdev
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;
	//now that we created cdev, we have to add it to the kernel
	//int cdev_add(struct cdev* dev, dev_t num, unsigned int count)
	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0){	//always check errors
		printk(KERN_ALERT "sysprogdevice: unable to add cdev to kernel");
		return ret;
	}

	//(4) Initialize our semaphore
	sema_init(&virtual_device.sem,1);	//initial value of one

	return 0;
}

static void driver_exit(void){
	//(5) unregister everything in reverse order
	//(a)
	cdev_del(mcdev);

	//(b)
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "sysprogdevice: unloaded module");
}


module_init(driver_entry);
module_exit(driver_exit);

MODULE_AUTHOR( "Tahrima Hashem" );
MODULE_LICENSE( "GPL" );
