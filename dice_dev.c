/*
 * "Dice Simulation!" minimal kernel module - /dev version
 *
 * Erik Roeckel <efr11@pitt.edu>
 *
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>

#include <asm/uaccess.h>


// file is indefinitley long , *ppos represents current position in file handled by kernel
static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	unsigned char* mybuf;
	int i = 0;
	mybuf = kmalloc(count, GFP_KERNEL); //dynamically allocates input buffer
	get_random_bytes(mybuf, count); // gets count random bytes and fills my buf with those bytes
	
	while(i < count)
	{
		mybuf[i] %= 6; //gives our random bytes the range from 0-5
		i++;
	}

	//we want to make sure we are reading in each byte at a time
	if (count < 1)
	{
		kfree(mybuf);
		return -EINVAL;
	}
	
	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer.
	 */
	if (copy_to_user(buf, mybuf, count))
	{
		kfree(mybuf);
		return -EINVAL;
	}
	
	//tells OS we moved count bytes through this file
	*ppos = *ppos + count;

	//always free memory that you allocate
	kfree(mybuf);

	return count;// returns count user asked for
}


// The only file operation we care about is read.
 
static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/dice.
	 */
	"dice",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	/*
	 * Create the "dice" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/dice device using
	 * the default rules.
	 */
	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR
		       "Unable to register \"Dice Game!\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Erik Roeckel <efr11@pitt.edu>");
MODULE_DESCRIPTION("\"Dice Roll Simulation!\" minimal module");
MODULE_VERSION("dev");
