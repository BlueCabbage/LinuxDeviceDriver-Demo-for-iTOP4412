/*************************************************************************
    > File Name: leds.c
    > Author: ZHAOCHAO
    > Mail: 479680168@qq.com 
    > Created Time: Thu 13 Oct 2016 06:29:14 AM PDT
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include <linux/fs.h>
#include <linux/stat.h>

#include <linux/gpio.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio.h>

#include <mach/gpio-exynos4.h>

#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <linux/types.h>



#define DRIVER_NAME "leds_ctl"
#define DEVICE_NAME	"char_dev_leds_ctl"

#ifndef DEV_MAJOR
	#define DEV_MAJOR	0
#endif

#ifndef DEV_MINOR
	#define DEV_MINOR	0
#endif

#define DEV_MINOR_NUM	1

#ifndef REGDEV_SIZE
	#define REGDEV_SIZE		3000
#endif

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("MRI_ZHAOCHAO");


struct reg_dev {
	char *cdat;
	unsigned long size;
	int  idat;

	struct cdev cdev;
};

static struct class *myClass;
static struct reg_dev *myDev;

int dev_major = DEV_MAJOR;
int dev_minor = DEV_MINOR;


static int char_node_leds_ops_open(struct inode *inode, struct file *file) 
{
	printk(KERN_INFO "open leds ... \n");

	return 0;
}

static int char_node_leds_ops_release(struct inode *inode, struct file *file) 
{
	printk(KERN_INFO "release leds ... \n");

	return 0;
}

static long char_node_leds_ops_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk(KERN_INFO "cmd: %d, arg: %ld\n", cmd, arg);

	if (cmd > 1 || cmd < 0) {
		printk(KERN_INFO "cmd is 0 or 1\n");
		goto fail;
	}
	if (arg > 1 || arg < 0) {
		printk(KERN_INFO "arg is 0 or 1\n");
		goto fail;
	}

	printk(KERN_INFO "set gpc0_2 %d\n", (int)arg);

//	gpio_set_value(EXYNOS4_GPL2(0), arg);
	gpio_set_value(EXYNOS4_GPC0(2), arg);

	return 0;
fail:
	return 1;
}

ssize_t char_node_leds_ops_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops) 
{
	return 0;
}

ssize_t char_node_leds_ops_write(struct file *file, char __user *buf, size_t count, loff_t *f_ops)
{
	return 0;
}

ssize_t char_node_leds_ops_llseek(struct file *file, loff_t offset, int ence) 
{
	return 0;
}

static struct file_operations char_dev_leds_ops = {
	.owner = THIS_MODULE,
	.open = char_node_leds_ops_open,
	.release = char_node_leds_ops_release,
	.unlocked_ioctl = char_node_leds_ops_ioctl,
	.read = char_node_leds_ops_read,
	.write = char_node_leds_ops_write,
	.llseek = char_node_leds_ops_llseek,
};

static void reg_init_cdev(struct reg_dev *dev, int index)
{
	int ret;

	int devno = MKDEV(dev_major, dev_minor + index);

	cdev_init(&dev->cdev, &char_dev_leds_ops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &char_dev_leds_ops;

	ret = cdev_add(&dev->cdev, devno, 1);
	if (ret) {
		printk(KERN_EMERG "cdev_add %d is failed ...\n", index);
	} else {
		printk(KERN_INFO "cdev_add %d is successed! ...\n", index);
	}
}


static int gpio_init(void)
{
	int ret = 0;	

//	ret = gpio_request(EXYNOS4_GPL2(0), "led0");
	ret = gpio_request(EXYNOS4_GPC0(2), "led0");
	if (ret < 0) {
		printk(KERN_EMERG "failed gpio_requedt EXYNOS_CPC0(2)!\n");
		goto fail;
	}

	s3c_gpio_cfgpin(EXYNOS4_GPC0(2), S3C_GPIO_OUTPUT);
	//s3c_gpio_cfgpin(EXYNOS4_GPL2(0), S3C_GPIO_OUTPUT);
//	gpio_set_value(EXYNOS4_GPL2(0), 0);
	gpio_set_value(EXYNOS4_GPC0(2), 0);

	if (ret < 0) {
		printk(KERN_EMERG "gpio config failed!\n");
		goto fail;
	}

	return ret;

fail:
	gpio_free(EXYNOS4_GPC0(2));
	return ret;
}

static int leds_plt_probe(struct platform_device *pdv) 
{
	int ret, i;

	printk(KERN_INFO "\tinitialized... \n");

	dev_t num_dev;

	printk(KERN_INFO "number_major: %d!\n", dev_major);
	printk(KERN_INFO "number_minor: %d!\n", dev_minor);

	if (dev_major) {
		num_dev = MKDEV(dev_major, dev_minor);
		ret = register_chrdev_region(num_dev, DEV_MINOR_NUM, DEVICE_NAME);
	} else {
		ret = alloc_chrdev_region(&num_dev, dev_minor, DEV_MINOR_NUM, DEVICE_NAME);
		dev_major = MAJOR(num_dev);

		printk(KERN_INFO "adev_region req %d  ...\n", dev_major);
	}
	if (ret < 0) {
		printk(KERN_EMERG "register_chrdev_region req %d is failed! ... \n", dev_major);
	}

	myClass = class_create(THIS_MODULE, DEVICE_NAME);

	myDev = kmalloc(DEV_MINOR_NUM * sizeof(struct reg_dev), GFP_KERNEL);
	if (!myDev) {
		ret = -ENOMEM;
		goto fail;
	}

	memset(myDev, 0, DEV_MINOR_NUM * sizeof(struct reg_dev));

	for (i = 0; i < DEV_MINOR_NUM; i ++) {

		printk(KERN_EMERG "dbg[%d.0]\n", i);
		myDev[i].cdat = kmalloc(REGDEV_SIZE, GFP_KERNEL);
		memset(myDev[i].cdat, 0, REGDEV_SIZE);

		reg_init_cdev(&myDev[i], i);
		printk(KERN_EMERG "dbg[%d.1]\n", i);

		device_create(myClass, NULL, MKDEV(dev_major, dev_minor), NULL, DEVICE_NAME"%d", i);
		printk(KERN_EMERG "dbg[%d.2]\n", i);
	}

	printk(KERN_INFO "cdev_create successed! \n");

	ret = gpio_init();
	if (ret < 0) {
		goto fail;
	}

	printk(KERN_INFO "probed! \n");

	return 0;

fail:
	unregister_chrdev_region(MKDEV(dev_major, dev_minor), DEV_MINOR_NUM);
	printk(KERN_EMERG "kmalloc is failed!\n");

	return ret;
}

static int leds_plt_remove(struct platform_device *pdev)
{
	int ret;

	printk(KERN_INFO "\tremoce...\n");
	if (ret < 0) {
		printk(KERN_EMERG "failed misc_deregister(&leds_misc_dev)");
		return ret;
	}

//	gpio_free(EXYNOS4_GPL2(0));
	gpio_free(EXYNOS4_GPC0(2));

	return 0;
}

static int leds_plt_shutdown(struct platform_device *pdev)
{
	printk(KERN_INFO "\tshutdown...\n");
	
	return 0;
}

static int leds_plt_suspend(struct platform_device *pdev, pm_message_t pmt) 
{
	printk(KERN_INFO "\tsuspend...\n");

	return 0;
}

static int leds_plt_resume(struct platform_device *pdev) 
{
	printk(KERN_INFO "\tresume...\n");

	return 0;
}

struct platform_driver leds_driver_plt = {
	.probe = leds_plt_probe,
	.remove = leds_plt_remove,
	.shutdown = leds_plt_shutdown,
	.suspend = leds_plt_suspend,
	.resume = leds_plt_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	}
};

static int leds_init(void)
{
	int _Driverstate;

	printk(KERN_EMERG "init leds ....\n");
	_Driverstate = platform_driver_register(&leds_driver_plt);
	printk(KERN_EMERG "\tinit state: %d;\n", _Driverstate);

	return 0;
}

static void leds_exit(void)
{
	int i;

	printk(KERN_EMERG "\texit state... \n");
	platform_driver_unregister(&leds_driver_plt);

	for (i = 0; i < DEV_MINOR_NUM; i ++) {
		cdev_del(&myDev[i].cdev);

		device_destroy(myClass, MKDEV(dev_major, dev_minor + i));
	}

	class_destroy(myClass);
	kfree(myDev);
	
	unregister_chrdev_region(MKDEV(dev_major, dev_minor), DEV_MINOR_NUM); 

	printk(KERN_INFO "exited ...\n");
}

module_init(leds_init);
module_exit(leds_exit);


