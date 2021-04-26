#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

static int major;
static struct class *cls;
static struct i2c_client *at24cxx_client;

/* 传入: buf[0] : addr
 * 输出: buf[0] : data
 */
static ssize_t at24cxx_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
	unsigned char address, data;
	copy_from_user (&address, buf, 1);
	data = i2c_smbus_read_byte_data (at24cxx_client, address);
	copy_to_user (buf, &data, 1);
}

/* buf[0] : addr
 * buf[1] : data
 */
static ssize_t at24cxx_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
	unsigned char ker_buf[2];
	unsigned char address, data;
	
	copy_from_user (ker_buf, buf, 2);
	address = ker_buf[0];
	data = ker_buf[1];
	
	printk("addr = 0x%02x, data = 0x%02x\n", address, data);
	
	if (!(i2c_smbus_write_byte_data (at24cxx_client, address, data)))
		return 2;
	else
		return -EIO;
}

static struct file_operations at24cxx_fops = {
	.owner = THIS_MODULE,
	.read  = at24cxx_read,
	.write = at24cxx_write,
};
	
static int __devinit at24cxx_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	at24cxx_client = client;
	
	major = register_chrdev (0, "at24cxx", &at24cxx_fops);
	cls = class_create (THIS_MODULE, "at24cxx");
	device_create (cls, NULL, MKDEV(major, 0), NULL, "at24cxx");

	return 0;
}

static int __devexit at24cxx_remove(struct i2c_client *client)
{
	device_destroy (cls, MKDEV(major, 0));
	class_destroy (cls);
	unregister_chrdev (major, "at24cxx");
	
	return 0;
}


/* 分配/设置 i2c_driver     */
static const struct i2c_device_id at24cxx_i2c_id[] = {
	{ "at24cxx", 0 },
	{ }
};

static struct i2c_driver at24cxx_i2c_driver = {
	.driver = {
		.name	= "at24cxx_i2c",
		.owner	= THIS_MODULE,
	},
	.probe		= at24cxx_probe,
	.remove		= __devexit_p(at24cxx_remove),
	.id_table	= at24cxx_i2c_id,
};

static int at24cxx_drv_init(void)
{
	/* 2. 注册 i2c_driver  */
	int ret;
	ret = i2c_add_driver(&at24cxx_i2c_driver);
	if (ret != 0)
		pr_err("Failed to register at24cxx I2C driver: %d\n", ret);

	
	return 0;
}

static void at24cxx_drv_exit(void)
{
	i2c_del_driver(&at24cxx_i2c_driver);
}

module_init(at24cxx_drv_init);
module_exit(at24cxx_drv_exit);
MODULE_LICENSE("GPL");

