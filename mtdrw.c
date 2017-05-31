/*
 * mtdrw.c
 *
 * Lodable kernel module to make the MTD device writable while this module is
 * loaded.
 *
 * This module is for Atmark Techno's Armadillo-4x0 series CPU board running
 * Linux v2.6.26.
 *
 * (C) 2016 Yuichiro Goto <goto.yuichiro@espark.co.jp>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/init.h>

#include <linux/mtd/mtd.h>

static u_int32_t mtd_flags_save;

static char *name;
module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "device name of the MTD device to be made writeable");

static int __init mtdrw_init(void)
{
	struct mtd_info *mtd;
	int mtd_index;
	int status = 0;

	if (name == NULL) {
		status = -EINVAL;
		goto error;
	}

	mtd = get_mtd_device_nm(name);
	if (IS_ERR(mtd)) {
		status = PTR_ERR(mtd);
		goto error;
	}

	mtd_flags_save = mtd->flags;
	mtd->flags |= MTD_WRITEABLE;
	mtd_index = mtd->index;
	put_mtd_device(mtd);

	if (!(mtd_flags_save & MTD_WRITEABLE)) {
		pr_info("mtd%d is writeable now\n", mtd_index);
	}

error:
	return (status);
}
module_init(mtdrw_init);

static void __exit mtdrw_exit(void)
{
	struct mtd_info *mtd;
	int mtd_index;

	/* Undo the change on flags */
	mtd = get_mtd_device_nm(name);
	if (mtd == NULL) {
		goto error;
	}

	mtd->flags = mtd_flags_save;
	mtd_index = mtd->index;
	put_mtd_device(mtd);

	if (!(mtd_flags_save & MTD_WRITEABLE)) {
		pr_info("mtd%d is read-only again\n", mtd_index);
	}

error:
	return;
}
module_exit(mtdrw_exit);

MODULE_AUTHOR("Yuichiro Goto <goto.yuichiro@espark.co.jp>");
MODULE_DESCRIPTION("Make the MTD device writeable");
MODULE_LICENSE("Dual BSD/GPL");
