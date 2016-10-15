/*************************************************************************
    > File Name: KeyButton_Interrupt.c
    > Author: ZHAOCHAO
    > Mail: 479680168@qq.com 
    > Created Time: Sat 15 Oct 2016 06:46:11 AM PDT
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <mach/regs-gpio.h>
#include <asm/io.h>
#include <linux/regulator/consumer.h>
#include <linux/delay.h>

#include <linux/interrupt.h>
#include <linux/irq.h>

/////////////////////////////////////////////////////////////////////////////////////////////////
///// BUTTON: 
/////         HOME -- GPX1_1 -- XEINT9
/////         BACK -- GPX1_2 -- XEINT10
/////         SLEEP -- GPX2_6 -- XEINT22

#define DRIVER_NAME		"leds_ctl"
#define DEVICE_NAME		"Button_irq"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("NMR_ZHAOCHAO");


static irqreturn_t eint9_interrupt(int irq, void *dev_id)
{
	printk(KERN_INFO "%s(%d) getted EINT9 ... \n", __FUNCTION__, __LINE__);

	return IRQ_HANDLED;
}

static irqreturn_t eint10_interrupt(int irq, void *dev_id)
{
	printk(KERN_INFO "%s(%d) getted EINT10 ... \n", __FUNCTION__, __LINE__);

	return IRQ_HANDLED;
}

static int irq_plt_probe(struct platform_device *pdev)
{
	int ret;

	printk(KERN_INFO "key_irq initialize... \n");

	ret = gpio_request(EXYNOS4_GPX1(1), "EINT9 request...\n");
	if (ret < 0 ) {
		printk(KERN_EMERG "gpio_request(gpx1_1 failed ...\n)");
		goto fail;
	}

	s3c_gpio_cfgpin(EXYNOS4_GPX1(1), S3C_GPIO_SFN(0XF));
	s3c_gpio_setpull(EXYNOS4_GPX1(1), S3C_GPIO_PULL_UP);
	gpio_free(EXYNOS4_GPX1(1));


	ret = gpio_request(EXYNOS4_GPX1(2), "EINT10 request...\n");
	if (ret < 0 ) {
		printk(KERN_EMERG "gpio_request(gpx1_2 failed ...\n)");
		goto fail;
	}

	s3c_gpio_cfgpin(EXYNOS4_GPX1(2), S3C_GPIO_SFN(0xF));
	s3c_gpio_setpull(EXYNOS4_GPX1(2), S3C_GPIO_PULL_UP);
	gpio_free(EXYNOS4_GPX1(2));


	ret = request_irq(IRQ_EINT(9), eint9_interrupt, IRQ_TYPE_EDGE_FALLING, "EINT9", pdev);
	if (ret < 0) {
		printk(KERN_EMERG "Request IRQ EINT9 failed ... \n");
		goto fail;
	}
	printk(KERN_INFO "requested irq eint9 ... \n");

	ret = request_irq(IRQ_EINT(10), eint10_interrupt, IRQ_TYPE_EDGE_FALLING, "EINT10", pdev);
	if (ret < 0) {
		printk(KERN_EMERG "Request IRQ EINT10 failed ... \n");
		goto fail;
	}
	printk(KERN_INFO "requested irq eint10 ... \n");

	return 0;

fail:
	return ret;
}


static int irq_plt_remove(struct platform_device *pdev)
{
	free_irq(IRQ_EINT(9), pdev);
	free_irq(IRQ_EINT(10), pdev);

	printk(KERN_INFO "irq_plt_removed ... \n");

	return 0;
}

static int irq_plt_suspend(struct platform_device *pdev)
{
	printk(KERN_INFO "irq_plt suspend ... [power off!] \n");

	return 0;
}

static int irq_plt_resume(struct platform_device *pdev)
{
	printk(KERN_INFO "irq_plt_resume ... [power on!]\n");
	
	return 0;
}

struct platform_driver key_irq_plt_driver = {
	.probe = irq_plt_probe,
	.remove = irq_plt_remove,
	.suspend = irq_plt_suspend,
	.resume = irq_plt_resume,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};

static void __init key_irq_init(void)
{
	printk(KERN_INFO "init ... \n");
	platform_driver_register(&key_irq_plt_driver);
	printk(KERN_INFO "inited ... \n");
}

module_init(key_irq_init);


static void __exit key_irq_exit(void)
{
	printk(KERN_INFO "exit ... \n");
	platform_driver_unregister(&key_irq_plt_driver);
	printk(KERN_INFO "exited ... \n");
}

module_exit(key_irq_exit);
