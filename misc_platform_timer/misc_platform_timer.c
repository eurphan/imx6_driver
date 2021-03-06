#include <linux/module.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/ide.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>

struct timer_dev {
	dev_t devid;				/* 设备号 */
	struct cdev cdev;			/* cdev */
	int major;					/* 主设备号 */
	int minor;					/* 次设备号 */
	struct timer_list timer;
	int timer_period;
};

struct timer_dev timer_device;		/* timer设备 */

/*
 * @description		: 驱动打开执行函数 
 * @param - inode	: 传递给驱动的inode
 * @param - filep	: 设备文件结构体指针
 * @return			: 0 成功; 其他 失败
 * */
static int timer_open(struct inode *inode, struct file *filep)
{
	filep->private_data = &timer_device;		/* 将timer设备文件地址赋给文件私有指针 */
	return 0;
}

/*
 * @description		: 从设备读取数据函数`
 * @param - filep	: 设备文件结构体指针
 * @param - buff	: 返回数据给用户空间的数据缓存
 * @param - cnt		: 需要读取的数据个数
 * @param - offt	: 读取位置相对于首地址的偏移
 * @return			: 实际读取的字节数，如果为负数代表读取失败
 * */
static ssize_t timer_read(struct file *filp, char __user *buff, size_t cnt, loff_t *offt)
{
	return 0;
}

/*
 * @description		: 向设备写入数据函数`
 * @param - filep	: 设备文件结构体指针
 * @param - buff	: 需要写入设备文件的用户空间数据缓存
 * @param - cnt		: 需要写入的数据个数
 * @param - offt	: 写入位置相对于首地址的偏移
 * @return			: 实际写入的字节数，如果为负数代表写入失败
 * */
static ssize_t timer_write(struct file *filp, char __user *buff, size_t cnt, loff_t *offt)
{
	return 0;
}

/*
 * @description		: 驱动关闭执行函数 
 * @param - inode	: 传递给驱动的inode
 * @param - filep	: 设备文件结构体指针
 * @return			: 0 成功; 其他 失败
 * */
static int timer_release(struct inode *inode, struct file *filep)
{
	return 0;
}


/* 操作函数结构体 */
static struct file_operations timer_fops = {
	.owner = THIS_MODULE,
	.open = timer_open,
	.read = timer_read,
	.write = timer_write,
	.release = timer_release,
};

static struct miscdevice timer_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "timer",
	.fops = &timer_fops,
};

static void timer_calback(unsigned long param)
{
	static int sec_count = 0;
	printk("Timer run %d second!\n", sec_count++);
	mod_timer(&timer_device.timer, jiffies + msecs_to_jiffies(timer_device.timer_period));
}

/*
 * @description		: 设备、驱动配对执行函数 
 * @param			: dev:设备
 * @return			: 无
 * */
static int timer_probe(struct platform_device* dev)
{
	int ret;
	/* 注册字符设备驱动 */
	/* 4.初始化并启动一个timer */
	timer_device.timer_period = 1000;
	init_timer(&timer_device.timer);
	timer_device.timer.function = timer_calback;
	timer_device.timer.data = (unsigned long)&timer_device;
	timer_device.timer.expires = jiffies + msecs_to_jiffies(timer_device.timer_period);
	add_timer(&timer_device.timer);

	ret = misc_register(&timer_miscdev);
	if (ret < 0) {
		printk("Failed to register timer misc driver!\n");
		return -EFAULT;
	}
	return 0;
}

/*
 * @description		: 设备、驱动移除执行函数 
 * @param			: dev:设备
 * @return			: 无
 * */
static int timer_remove(struct platform_device* dev)
{
	del_timer(&timer_device.timer);
	misc_deregister(&timer_miscdev);
	return 0;
}

static const struct of_device_id timer_of_match[] = {
	{.compatible = "imx6ul_timer"},
	{},
};

static struct platform_driver imx6ul_timer_driver = {
	.driver = {
		.name = "imx6ul_timer",
		.of_match_table = timer_of_match,
	},
	.probe = timer_probe,
	.remove = timer_remove,
};

/*
 * @description		: 驱动加载执行函数 
 * @param			: 无
 * @return			: 无
 * */
static __init int timer_init(void)
{
	platform_driver_register(&imx6ul_timer_driver);
	return 0;
}

/*
 * @description		:驱动卸载执行函数
 * @param			:无
 * @return			:无
 */
static __exit void timer_exit(void)
{
	platform_driver_unregister(&imx6ul_timer_driver);
}

module_init(timer_init);
module_exit(timer_exit);


MODULE_AUTHOR("eurphan<eurphan@163.com>");
MODULE_DESCRIPTION("Timer test driver");
MODULE_LICENSE("GPL");

