/*
 * Microchip I2C Touchscreen Driver
 *
 * Copyright (c) 2011 Microchip Technology, Inc.
 *
 * http://www.microchip.com/mtouch
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>

static int calibration[12];
module_param_array(calibration, int, NULL, S_IRUGO | S_IWUSR | S_IWGRP);

#define AR1020_I2C_HEADER	0x55
#define AR1020_I2C_TRP_SZ	5	/* The maximum packet byte length for Touch Reporting Protocol */
#define AR1020_I2C_CP_SZ	14	/* The maximum packet byte length for Command Protocol */

#define AR1020_X_RAW_MAX	4095	/* Max value sent by controler for X */
#define AR1020_Y_RAW_MAX	4095	/* Max value sent by controler for Y */

static void translate(int *px, int *py)
{
	if (calibration[0] || calibration[1] || calibration[2]) {
		int x = *px;
		int y = *py;
		u32 s[6];
		s64 xsum, ysum;
		int i;

		s[0] = 1 << 16;
		s[1] = x << (16 - 12);
		s[2] = y << (16 - 12);
		s[3] = (s[1] * s[2]) >> 16;
		s[4] = (s[1] * s[1]) >> 16;
		s[5] = (s[2] * s[2]) >> 16;

		xsum = 0;
		ysum = 0;
		for (i = 0; i < 6 ; i++) {
			xsum += (calibration[i] * (s64)s[i]);
			ysum += (calibration[i + 6] * (s64)s[i]);
		}
		x = (s32)(xsum >> (32 - 12));
		y = (s32)(ysum >> (32 - 12));
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		if (x > AR1020_X_RAW_MAX)
			x = AR1020_X_RAW_MAX;
		if (y > AR1020_Y_RAW_MAX)
			y = AR1020_Y_RAW_MAX;
		*px = x ;
		*py = y ;
	}
}

/* The private data structure that is referenced within the I2C bus driver */
struct ar1020_i2c_priv {
	struct i2c_client *client;
	struct input_dev *input;
	struct work_struct work;

	int command_pending;
	int button;
};

/*
 * These are all the sysfs variables used to store and retrieve information
 * from a user-level application
 */
static char sendBuffer[128];
static char receiveBuffer[128];
static int commandDataPending=0;

/*
 * These variables allows the IRQ to be specified via a module parameter
 * or kernel parameter.  To configuration of these value, please see
 * driver documentation.
 */
static int touchIRQ;

module_param(touchIRQ, int, S_IRUGO);


/*
 * Since the reference to private data is stored within the I2C
 * bus driver, we will store another reference within this driver
 * so the sysfs related function may also access this data
 */
struct ar1020_i2c_priv *g_priv;

/******************************************************************************
Function:
	commandDataPending_show()

Description:
	Display value of "commandDataPending" variable to application that is
	requesting it's value.
******************************************************************************/
static ssize_t commandDataPending_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", commandDataPending);
}

/******************************************************************************
Function:
	commandDataPending_store()

Description:
	Save value to "commandDataPending" variable from application that is
	requesting this.
******************************************************************************/
static ssize_t commandDataPending_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	sscanf(buf, "%d", &commandDataPending);
	return count;
}

static struct kobj_attribute commandDataPending_attribute =
	__ATTR(commandDataPending, 0664, commandDataPending_show,
			commandDataPending_store);

/******************************************************************************
Function:
	receiveBuffer_show()

Description:
	Display value of "receiveBuffer" variable to application that is
	requesting it's value.
******************************************************************************/
static ssize_t receiveBuffer_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	/* since we have now read the receiveBuffer, receive data is no longer pending */
	commandDataPending = 0;
	return sprintf(buf, "%s", receiveBuffer);
}

/******************************************************************************
Function:
	receiveBuffer_store()

Description:
	Save value to "receiveBuffer" variable from application that is
	requesting this.
******************************************************************************/
static ssize_t receiveBuffer_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	return snprintf(receiveBuffer, sizeof(receiveBuffer), "%s", buf);
}

static struct kobj_attribute receiveBuffer_attribute =
	__ATTR(receiveBuffer, 0664, receiveBuffer_show, receiveBuffer_store);

/******************************************************************************
Function:
	sendBuffer_show()

Description:
	Display value of "sendBuffer" variable to application that is
	requesting it's value.
******************************************************************************/
static ssize_t sendBuffer_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s", sendBuffer);
}

/******************************************************************************
Function:
	sendBuffer_store()

Description:
	Save value to "sendBuffer" variable from application that is
	requesting this.
******************************************************************************/
static ssize_t sendBuffer_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	int data[8];
	char buff[8];
	int cnt;
	int ret;
	int i;
	struct ar1020_i2c_priv *priv = g_priv;

	commandDataPending = 0;

	cnt = sscanf(buf, "%x %x %x %x %x %x %x %x", &data[0], &data[1],
		&data[2], &data[3], &data[4], &data[5], &data[6], &data[7]);

	pr_debug("AR1020 I2C: Processed %d bytes.\n", cnt);

	/* Verify command string to send to controller is valid */
	if (cnt < 3)
		pr_info("AR1020 I2C: Insufficient command bytes to process.\n");
	else if (data[0] != 0x0)
		pr_info("AR1020 I2C: Leading zero required when sending I2C commands.\n");
	else if (data[1] != 0x55)
		pr_info("AR1020 I2C: Invalid header byte (0x55 expected).\n");
	else if (data[2] != (cnt - 3))
		pr_info("AR1020 I2C: Number of command bytes specified not "
				"valid for current string.\n");

	memset(sendBuffer,0, sizeof(sendBuffer));
	pr_debug("AR1020 I2C: sending command bytes: ");
	for (i = 0; i < cnt; i++) {
		buff[i] = (char)data[i];
		pr_debug("0x%02x ", data[i]);
	}
	pr_debug("\n");

	if (priv) {
		ret = i2c_master_send(priv->client, buff, cnt);
		if (ret < 0)
			pr_debug("AR1020 I2C: i2c_master_send failed with %d", ret);
		else
			priv->command_pending = 1;
	}

	return snprintf(sendBuffer, sizeof(sendBuffer), "%s", buf);
}

static struct kobj_attribute sendBuffer_attribute =
	__ATTR(sendBuffer, 0664, sendBuffer_show, sendBuffer_store);

/*
 * Create a group of calibration attributes so we may work with them
 * as a set.
 */
static struct attribute *attrs[] = {
	&commandDataPending_attribute.attr,
	&receiveBuffer_attribute.attr,
	&sendBuffer_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static struct kobject *ar1020_kobj;

static irqreturn_t touch_irq(int irq, void *dev_id);

/******************************************************************************
Function:
	ar1020_i2c_readdata()

Description:
	When the controller interrupt is asserted, this function is scheduled
	to be called to read the controller data within the
	touch_irq() function.
******************************************************************************/
static void ar1020_i2c_readdata(struct work_struct *work)
{
	struct ar1020_i2c_priv *priv = container_of(work, struct ar1020_i2c_priv, work);

	unsigned char packet[128];
	int cnt;
	int i;

	cnt = i2c_master_recv(priv->client, packet, 1);

	if (cnt != 1) {
		pr_err("AR1020 I2C: error while i2c readdata\n");
		return;
	}

	/*
	 * Decode command packet.
	 * Data format, N bytes: 0x55, PACKET_BYTES, RESULT, CMD, DATA1, DATA2, DATA3, DATA4, ...
	 *   where the total length (N) is PACKET_BYTES+2
	 *     and the number of datapackets is PACKET_BYTES-2
	 *     and RESULT is the the command result
	 *     and CMD is the the command requested
	 */
	if (AR1020_I2C_HEADER == packet[0]) {
		cnt += i2c_master_recv(priv->client, &packet[1], 1);
		if (cnt != 2) {
			pr_err("AR1020 I2C: error while i2c cmd packet len\n");
			return;
		}

		if ((packet[1] <= 0) || (AR1020_I2C_CP_SZ < packet[1])) {
			pr_info("AR1020 I2C: invalid command packet size\n");
			goto error;
		}

		cnt += i2c_master_recv(priv->client, &packet[2], packet[1]);
		if (cnt != packet[1]+2) {
			pr_info("AR1020 I2C: invalid command packet size\n");
			return;
		}

		/* process command response */
		if (priv->command_pending) {
			unsigned char *p = receiveBuffer;
			unsigned char *pend = &p[sizeof(receiveBuffer) - 2];

			priv->command_pending = 0;

			for (i = 0; i < cnt; i++) {
				int ret = snprintf(p, pend - p, " 0x%02x", packet[i]);
				if (ret <= 0)
					break;

				p += ret;
				if (p >= pend)
					break;
			}

			*p++ = '\n';
			*p++ = 0;

			pr_info("AR1020 I2C: command response: %s", receiveBuffer);
			commandDataPending = 1;
		}
	}

	/*
	 * Decode packets of data from a device path using AR1XXX protocol.
	 * Data format, 5 bytes: SYNC, DATA1, DATA2, DATA3, DATA4
	 * SYNC [7:0]: 1,0,0,0,0,TOUCHSTATUS[0:0]
	 * DATA1[7:0]: 0,X-LOW[6:0]
	 * DATA2[7:0]: 0,X-HIGH[4:0]
	 * DATA3[7:0]: 0,Y-LOW[6:0]
	 * DATA4[7:0]: 0,Y-HIGH[4:0]
	 *
	 * TOUCHSTATUS: 0 = Touch up, 1 = Touch down
	 */
	else {
		int x;
		int y;
		int button;

		/* irq line may be shorted high, let's delay */
		if ((packet[0] & 0xfe) != 0x80) {
			pr_err("AR1020 I2C: 1st Touch byte not valid.  Value: 0x%02x\n", packet[0]);
			goto error;
		}

		cnt += i2c_master_recv(priv->client, &packet[1], AR1020_I2C_TRP_SZ-cnt);
		if (AR1020_I2C_TRP_SZ != cnt) {
			pr_info("AR1020 I2C: invalid touch packet size\n");
			return;
		}

		/* verify byte is valid for current index */
		for (i = 1; i < 5; i++) {
			if (0x80 & packet[i]) {
				pr_err("AR1020 I2C: Touch byte not valid.  Value: 0x%02x Index: 0x%02x\n", packet[i], i);
				goto error;
			}
		}

		button = (packet[0] & 1);

		/* do not process two button up presses in a row */
		if (button || priv->button) {
			priv->button = button;

			x = ((packet[2] & 0x1f) << 7) | (packet[1] & 0x7f);
			y = ((packet[4] & 0x1f) << 7) | (packet[3] & 0x7f);
			translate(&x, &y);

			input_report_abs(priv->input, ABS_X, x);
			input_report_abs(priv->input, ABS_Y, y);
			input_report_abs(priv->input, ABS_PRESSURE, button);
			input_report_key(priv->input, BTN_TOUCH, button);
			input_sync(priv->input);
		}
	}

	return;

error:

	/* attempt to repair the i2c bus packet stream by reading all pending data */
	i2c_master_recv(priv->client, packet, sizeof(packet));
}

/******************************************************************************
Function:
	ar1020_i2c_probe()

Description:
	After the kernel's platform specific source files have been modified to
	reference the "ar1020_i2c" driver, this function will then be called.
	This function needs to be called to finish registering the driver.
******************************************************************************/
static int ar1020_i2c_probe(struct i2c_client *client,
				      const struct i2c_device_id *id)
{
	struct ar1020_i2c_priv *priv = NULL;
	struct input_dev *input_dev = NULL;

	int err = 0;

	pr_info("%s: begin\n", __func__);

	if (!client) {
		pr_err("AR1020 I2C: client pointer is NULL\n");
		err = -EINVAL;
		goto error;
	}

	if (!client->irq) {
		pr_err("AR1020 I2C: client irq is NULL\n");
		err = -EINVAL;
		goto error;
	}

	priv = kzalloc(sizeof(struct ar1020_i2c_priv), GFP_KERNEL);
	if (!priv) {
		pr_err("AR1020 I2C: kzalloc error\n");
		err = -ENOMEM;
		goto error;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		pr_err("AR1020 I2C: input allocate error\n");
		err = -ENOMEM;
		goto error;
	}

	priv->client = client;
	priv->input = input_dev;
	priv->command_pending = 0;
	priv->button = 0;

	INIT_WORK(&priv->work, ar1020_i2c_readdata);
	
	input_dev->name = "AR1020 Touchscreen";
	input_dev->id.bustype = BUS_I2C;

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
	__set_bit(ABS_X, input_dev->absbit);
	__set_bit(ABS_Y, input_dev->absbit);
	__set_bit(ABS_PRESSURE, input_dev->absbit);
	__set_bit(EV_SYN, input_dev->evbit);


	input_set_abs_params(input_dev, ABS_X, 0, AR1020_X_RAW_MAX, 0, 0);
	input_set_abs_params(input_dev, ABS_Y, 0, AR1020_Y_RAW_MAX, 0, 0);
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 1, 0, 0);

	input_set_drvdata(input_dev, priv);
	err = input_register_device(input_dev);
	if (err) {
		pr_err("AR1020 I2C: error registering input device\n");
		goto error;
	}

	/* set type and register gpio pin as our interrupt */
	irq_set_irq_type(client->irq, IRQ_TYPE_EDGE_RISING);
	err = request_irq(client->irq, touch_irq, 0, "AR1020 I2C IRQ", priv);
	if (err < 0)
		goto error1;
	i2c_set_clientdata(client, priv);
	/*
	 * save pointer so sysfs helper functions may also have access
	 * to private data
	 */
	g_priv = priv;
	return 0;

error1:
	input_unregister_device(input_dev);
	
error:

	if (input_dev)
		input_free_device(input_dev);

	if (priv)
		kfree(priv);

	return err;
}

/******************************************************************************
Function:
	ar1020_i2c_remove()

Description:
	Unregister/remove the kernel driver from memory.
******************************************************************************/
static int ar1020_i2c_remove(struct i2c_client *client)
{
	struct ar1020_i2c_priv *priv =
			(struct ar1020_i2c_priv *)i2c_get_clientdata(client);

	free_irq(client->irq, priv);
	input_unregister_device(priv->input);
	kfree(priv);
	g_priv = NULL;
	return 0;
}

/* This structure describe a list of supported slave chips */
static const struct i2c_device_id ar1020_i2c_id[] = {
	{ "ar1020_i2c", 0 },
	{ }
};

/******************************************************************************
Function:
	touch_irq()

Description:
	After the interrupt is asserted for the controller, this
	is the first function that is called.  Since this is a time sensitive
	function, we need to immediately schedule work so the integrity of
	properly system operation

	This function needs to be called to finish registering the driver.
******************************************************************************/
static irqreturn_t touch_irq(int irq, void *dev_id)
{
	struct ar1020_i2c_priv *priv = (struct ar1020_i2c_priv *)dev_id;
	int err;

	if (!priv) {
		pr_err("AR1020 I2C: no private data\n");
		err = -EINVAL;
		return err;
	}

	 /* delegate I2C transactions since hardware interupts need to be handled very fast */
	schedule_work(&priv->work);

	return IRQ_HANDLED;
}

MODULE_DEVICE_TABLE(i2c, ar1020_i2c_id);

/*
 * This is the initial set of information the kernel has
 * before probing drivers on the system
 */
static struct i2c_driver ar1020_i2c_driver = {
	.driver = {
		.name	= "ar1020_i2c",
	},
	.probe		= ar1020_i2c_probe,
	.remove		= ar1020_i2c_remove,
	/*
	 * suspend/resume functions not needed since controller automatically
	 * put's itself to sleep mode after configurable short period of time
	 */
	.id_table	= ar1020_i2c_id,
};

/*
 * This function is called during startup even if the platform specific
 * files have not been setup yet.
 */
static int __init ar1020_i2c_init(void)
{
	int retval;

	pr_debug("AR1020 I2C: ar1020_i2c_init: begin\n");
	memset(receiveBuffer,0, sizeof(receiveBuffer));
	memset(sendBuffer,0, sizeof(sendBuffer));

	/*
	 * Creates a kobject "ar1020" that appears as a sub-directory
	 * under "/sys/kernel".
	 */
	ar1020_kobj = kobject_create_and_add("ar1020", kernel_kobj);
	if (!ar1020_kobj) {
		pr_err("AR1020 I2C: cannot create kobject\n");
		return -ENOMEM;
	}

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(ar1020_kobj, &attr_group);
	if (retval) {
		pr_err("AR1020 I2C: error registering ar1020-i2c driver's sysfs interface\n");
		kobject_put(ar1020_kobj);
	}

	return i2c_add_driver(&ar1020_i2c_driver);
}

/*
 * This function is called after ar1020_i2c_remove() immediately before
 * being removed from the kernel.
 */
static void __exit ar1020_i2c_exit(void)
{
	pr_debug("AR1020 I2C: ar1020_i2c_exit begin\n");
	kobject_put(ar1020_kobj);
	i2c_del_driver(&ar1020_i2c_driver);
}

MODULE_AUTHOR("Steve Grahovac <steve.grahovac@microchip.com>");
MODULE_DESCRIPTION("AR1020 touchscreen I2C bus driver");
MODULE_LICENSE("GPL");

/* Enable the ar1020_i2c_init() to be run by the kernel during initialization */
module_init(ar1020_i2c_init);

/* Enables the ar1020_i2c_exit() to be called during cleanup.  This only
   has an effect if the driver is compiled as a kernel module. */
module_exit(ar1020_i2c_exit);

