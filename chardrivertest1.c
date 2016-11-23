#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

DECLARE_WAIT_QUEUE_HEAD(WaitQ);
DECLARE_WAIT_QUEUE_HEAD(WaitQ_read);
DECLARE_WAIT_QUEUE_HEAD(WaitQ_write);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 32

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;
static int temp;

static int open_for_read=0;
static int open_for_write=0;
static struct file_operations fops = {
.read = device_read,
.write = device_write,
.open = device_open,
.release = device_release
};


int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}
	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");
	return SUCCESS;
}

void cleanup_module(void)
{

	unregister_chrdev(Major, DEVICE_NAME);
	/*if (ret < 0)
	printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);*/
}

static int device_open(struct inode *inode, struct file *file)
{
	
	if (Device_Open && (file->f_flags & O_NONBLOCK))
		return -EBUSY;
	printk(KERN_INFO "opened by minor: %d\n", MINOR(inode->i_rdev));
	int minor=MINOR(inode->i_rdev);
	while (Device_Open) {
		int i, is_sig = 0;
		if((!minor && open_for_write) || (minor && open_for_read)){
			wait_event_interruptible(WaitQ, !Device_Open);
			for (i = 0; i < _NSIG_WORDS && !is_sig; i++)
				is_sig =current->pending.signal.sig[i] & ~current->blocked.sig[i];
			if (is_sig) {
				module_put(THIS_MODULE);
				return -EINTR;
			}
		}
		else
			break;
	}

	//static int counter = 0;
	Device_Open++;
	msg_Ptr = msg;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "released by minor: %d\n", MINOR(inode->i_rdev));

	Device_Open--;
/* We're now ready for our next caller */
/*
* Decrement the usage count, or else once you opened the file, you'll
* never get get rid of the module.
*/
	wake_up(&WaitQ);
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t device_read(struct file *filp, char *buffer,size_t length,loff_t * offset){

	temp = MINOR((filp->f_inode)->i_rdev);
	printk(KERN_INFO "read request by minor: %d\n", temp);

	if(temp==1) {

		int bytes_read = 0;
		open_for_read=1;
		while(strlen(msg)<length && !(filp->f_flags & O_NONBLOCK)){
			int i, is_sig = 0;
			wait_event_interruptible(WaitQ_read, (strlen(msg)>=length));
			for (i = 0; i < _NSIG_WORDS && !is_sig; i++)
				is_sig =current->pending.signal.sig[i] & ~current->blocked.sig[i];
			if (is_sig) {
				module_put(THIS_MODULE);
				open_for_read=0;
				wake_up(&WaitQ_write);
				return -EINTR;
			}
		}
		if (*msg_Ptr == 0 || !strlen(msg))
			return 0;
		while (length && *msg_Ptr) {
			put_user(*(msg_Ptr++), buffer++);
			length--;
			bytes_read++;
		}
		if(bytes_read)
			strcpy(msg,msg+bytes_read);
		open_for_read=0;
		wake_up(&WaitQ_write);
		printk(KERN_INFO "Now buffer contains %d bytes, content= %s\n",strlen(msg), msg );
		return bytes_read;
	}
	else {
		return -1;
	}


}

static ssize_t device_write(struct file *filp, const char __user * buffer, size_t length, loff_t * offset)
{

	temp = MINOR((filp->f_inode)->i_rdev);
	printk(KERN_INFO "write request by minor: %d\n", temp); 
	
	if(temp == 0) {
		open_for_write=1;
		int j=strlen(msg);
		printk(KERN_INFO "written from offset: %d\n", j);
		int i;
		for (i=0; i < length && j < BUF_LEN; i++,j++){
			get_user(msg[j], buffer + i);
			wake_up(&WaitQ_read);
			while(strlen(msg)==BUF_LEN &&  !(filp->f_flags & O_NONBLOCK)){
				int x, is_sig = 0;
				wait_event_interruptible(WaitQ_write, (strlen(msg)<BUF_LEN));
				for (x = 0; x < _NSIG_WORDS && !is_sig; x++)
					is_sig =current->pending.signal.sig[x] & ~current->blocked.sig[x];
				if (is_sig) {
					module_put(THIS_MODULE);
					open_for_write=0;
					wake_up(&WaitQ_read);
					return -EINTR;
				}
			}

		}
		msg_Ptr = msg;
		open_for_write=0;
		wake_up(&WaitQ_read);
		printk(KERN_INFO "Now buffer contains %d bytes, content= %s\n",strlen(msg), msg_Ptr );
		return i;	
	}

	else {
		return -1;
	}
}
