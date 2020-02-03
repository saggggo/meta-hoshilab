#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

#define NUMBEROF_SYSFS_ENTRY 64

static struct kobject *hoshilab_kobj;
static void __iomem *base_addr;

static ssize_t config_show(struct kobject *kobj, struct kobj_attribute *kattr, char *buf) {
	return sprintf(buf, "%x: %x\n", base_addr, *(u32 *)base_addr);
}

static struct attribute *config_attrs[NUMBEROF_SYSFS_ENTRY + 1];
static struct attribute *data_attrs[NUMBEROF_SYSFS_ENTRY + 1];

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
	
	config_sysfs = (struct container_sysfs *)kzalloc(sizeof(struct container_sysfs) * NUMBEROF_SYSFS_ENTRY, GFP_KERNEL);

	for (i = 0; i < NUMBEROF_SYSFS_ENTRY; i++) {
		char name[3];
		name[0] = (char)(i/10 + '0');
		name[1] = (char)(i%10 + '0');
		name[2] = NULL;
		printk("%d: %s\n", i, name);
		
		config_sysfs[i].address = base_addr + i;
		config_sysfs[i].kattr.attr.name = name;
		config_sysfs[i].kattr.attr.mode = 0444;
		config_sysfs[i].kattr.show =  config_show;
		config_sysfs[i].kattr.store = NULL;

		config_attrs[i] = &config_sysfs[i].kattr.attr;
	}
	printk("%d:\n", i);
	config_attrs[i] = NULL;

	hoshilab_kobj = kobject_create_and_add("pe-platform", kernel_kobj);
	if (!hoshilab_kobj) {
		return -ENOMEM;
	}
	retval = sysfs_create_group(hoshilab_kobj, &config);
	printk("77: \n");
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
