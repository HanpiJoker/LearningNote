/* 参考 ./drivers/input/keyboard/gpio_keys.c */
#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/gpio_keys.h>

#include <asm/gpio.h>

struct pin_desc{
	int irq;
	char *name;
	unsigned int pin;
	unsigned int key_val;
};

struct pin_desc pin_descs[4] = {
	{IRQ_EINT0, "S2", S3C2410_GPF0, KEY_L},
	{IRQ_EINT2, "S3", S3C2410_GPF2, KEY_S},
	{IRQ_EINT11, "S4", S3C2410_GPG3, KEY_ENTER},
	{IRQ_EINT19, "S5", S3C2410_GPG11, KEY_LEFTSHIFT},
};

static struct input_dev *button_dev;
static struct pin_desc *irq_pd;
static struct timer_list buttons_timer;

static irqreturn_t buttons_irq (int irq, void *dev_id)
{
	irq_pd = (struct pin_desc *)dev_id;
	mod_timer (&buttons_timer, jiffies + HZ / 100);

	return IRQ_RETVAL (IRQ_HANDLED);
}


static void buttons_timer_function(unsigned long data)
{
	struct pin_desc *pindesc = irq_pd;
	unsigned int pinval; 

	pinval = s3c2410_gpio_getpin (pindesc->pin);

	if (pinval) {
		input_event (button_dev, EV_KEY, pindesc->key_val, 0);
		input_sync (button_dev);
	} else {
		input_event (button_dev, EV_KEY, pindesc->key_val, 1);
		input_sync (button_dev);
	}
}

static int buttons_init (void)
{
	int i;
	/* 1. 分配一个 input_dev 结构体 */
	if (!(button_dev = input_allocate_device ()))
		return -ENOMEM;
	
	/* 2. 设置 */
	/* 2.1 能产生哪一类事件 */
	set_bit (EV_KEY, button_dev->evbit);
	set_bit (EV_REP, button_dev->evbit);

	/* 2.2 能产生这一类哪一些事件 */
	set_bit (KEY_L, button_dev->keybit);
	set_bit (KEY_S, button_dev->keybit);
	set_bit (KEY_ENTER, button_dev->keybit);
	set_bit (KEY_LEFTSHIFT, button_dev->keybit);
	
	/* 3. 注册 */
	input_register_device (button_dev);
	/* 4. 硬件相关操作 */
	init_timer (&buttons_timer);
	buttons_timer.function = buttons_timer_function;

	add_timer (&buttons_timer);
	for(i = 0; i < 4; i++) {
		request_irq (pin_descs[i].irq,  buttons_irq, IRQT_BOTHEDGE
		, pin_descs[i].name, &pin_descs[i]);
	}
	return 0;
}

static void buttons_exit (void) 
{
	int i;
	for (i = 0; i < 4; i++)	
		free_irq (pin_descs[i].irq, &pin_descs[i]);
	
	del_timer (&buttons_timer);
	input_unregister_device (button_dev);
	input_free_device (button_dev);
}

module_init (buttons_init);
module_exit (buttons_exit);
MODULE_LICENSE("GPL");

