/*************************************************************************
    > File Name: leds.c
    > Author: ZHAOCHAO
    > Mail: 479680168@qq.com 
    > Created Time: Thu 13 Oct 2016 06:29:14 AM PDT
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>

#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include <linux/fs.h>

#include <linux/gpio.h>

#include <plat/gpio-cfg.h>
#include <mach/gpio.h>

#include <mach/gpio-exynos4.h>



#define DRIVER_NAME "leds_ctl"
#define DEVICE_NAME	"leds_ctl_dev"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("MRI_ZHAOCHAO");

static int leds_ops_open(struct inode *inode, struct file *file) 
{
	printk(KERN_INFO "open leds ... \n");

	return 0;
}

static int leds_ops_release(struct inode *inode, struct file *file) 
{
	printk(KERN_INFO "release leds ... \n");

	return 0;
}

static long leds_ops_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
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

	printk(KERN_INFO "set gpc0_2 %d\n", arg);

//	gpio_set_value(EXYNOS4_GPL2(0), arg);
	gpio_set_value(EXYNOS4_GPC0(2), arg);

	return 0;
fail:
	return 1;
}

static struct file_operations leds_ops = {
	.owner = THIS_MODULE,
	.open = leds_ops_open,
	.release = leds_ops_release,
	.unlocked_ioctl = leds_ops_ioctl,
};

static struct miscdevice leds_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &leds_ops,
};


static int leds_plt_probe(struct platform_device *pdv) 
{
	int ret;

	printk(KERN_INFO "\tinitialized... \n");

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

	ret = misc_register(&leds_misc_dev);
	if (ret < 0) {
		printk(KERN_EMERG "failed misc_register(&leds_misc_dev)");
		goto fail;
	}


	return 0;

fail:
	misc_deregister(&leds_misc_dev);
	return ret;
}

static int leds_plt_remove(struct platform_device *pdev)
{
	int ret;

	printk(KERN_INFO "\tremoce...\n");
	ret = misc_deregister(&leds_misc_dev);
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
	
	platform_driver_unregister(&leds_driver_plt);
	printk(KERN_EMERG "\texit state... \n");

}

module_init(leds_init);
module_exit(leds_exit);


