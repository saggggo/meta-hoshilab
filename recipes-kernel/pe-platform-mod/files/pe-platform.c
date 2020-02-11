#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define NUMBEROF_CONFIG_ENTRY 64

static struct kobject *hoshilab_kobj;
static void __iomem *base_addr;

static struct attribute *config_attrs[NUMBEROF_CONFIG_ENTRY + 1];
static struct attribute *data_attrs[NUMBEROF_CONFIG_ENTRY + 1];

static struct attribute_group config = {
	.name = "config",
	.attrs = config_attrs,
};

static struct attribute_group data = {
	.name = "data",
	.attrs = data_attrs
};

struct container_sysfs {
	void *address;
	struct kobj_attribute kattr;
};

static struct container_sysfs *config_sysfs;
static char (*config_name)[3];
static char (*data_name)[3];

static ssize_t config_read(struct kobject *kobj, struct kobj_attribute *kattr, char *buf) {
	struct container_sysfs *container;
	container = container_of(kattr, struct container_sysfs, kattr);
	return sprintf(buf, "%p: %x\n", container->address, *(u32 *)container->address);
}

static ssize_t config_write(struct kobject *kobj, struct kobj_attribute *kattr, const char *buf, size_t size) {
	u32 value;
	int num;
	struct container_sysfs *container;
	container = container_of(kattr, struct container_sysfs, kattr);
	num = sscanf(buf, "%x", &value);
	if (!num)
		return -1;
	*(u32 *)(container->address) = value;
	return size;
}

static ssize_t data_read(struct kobject *kobj, struct kobj_attribute *kattr, char *buf) {
	struct container_sysfs *container;
	container = container_of(kattr, struct container_sysfs, kattr);
	return sprintf(buf, "%p: %x\n", container->address, *(u32 *)container->address);
}

struct hoshilab_pe_platform_data {
	const char *name;
} pe_platform_data = {"pe-platform"};

static const struct of_device_id hoshilab_pe_platform_of_match[] = {
	{ .compatible = "hoshilab,pe-platform",
	  .data = &pe_platform_data},
	{},
};

static int pe_platform_probe(struct platform_device *pdev) {
	int i;
	int retval;
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base_addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(base_addr))
			return PTR_ERR(base_addr);

	config_sysfs = (struct container_sysfs *)kzalloc(sizeof(struct container_sysfs) * NUMBEROF_CONFIG_ENTRY, GFP_KERNEL);
	config_name = (char(*)[3])kzalloc(sizeof(char)*3*NUMBEROF_CONFIG_ENTRY, GFP_KERNEL);
	for (i = 0; i < NUMBEROF_CONFIG_ENTRY; i++) {
		config_name[i][0] = (char)(i/10 + '0');
		config_name[i][1] = (char)(i%10 + '0');
		config_name[i][2] = '\0';

		config_sysfs[i].address = base_addr + i*4;
		config_sysfs[i].kattr.attr.name = config_name[i];
		config_sysfs[i].kattr.attr.mode = 0644;
		config_sysfs[i].kattr.show =  config_read;
		config_sysfs[i].kattr.store = config_write;
		config_attrs[i] = &config_sysfs[i].kattr.attr;
	}
	config_attrs[i] = NULL;

	hoshilab_kobj = kobject_create_and_add("pe-platform", kernel_kobj);
	if (!hoshilab_kobj) {
		return -ENOMEM;
	}
	retval = sysfs_create_group(hoshilab_kobj, &config);
	if (retval) {
		kobject_put(hoshilab_kobj);
	}
	return retval;
}

static int pe_platform_remove(struct platform_device *pdev) {
	kobject_put(hoshilab_kobj);
	return 0;
}

static struct platform_driver pe_platform_driver = {
	.probe = pe_platform_probe,
	.remove = pe_platform_remove,
	.driver = {
		.name = "hoshilab_pe_platform",
		.of_match_table = of_match_ptr(hoshilab_pe_platform_of_match),
	},
};

module_platform_driver(pe_platform_driver);

MODULE_DESCRIPTION("hoshilab config");
MODULE_AUTHOR("Saggggo");
MODULE_LICENSE("GPL v2");
