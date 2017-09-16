#include <mach/board.h>
#include <asm/setup.h>
#include <linux/mtd/nand.h>
#include <linux/module.h>

static int mfg_mode;
int __init board_mfg_mode_init(char *s)
{
	if (!strcmp(s, "normal"))
		mfg_mode = 0;
	else if (!strcmp(s, "factory2"))
		mfg_mode = 1;
	else if (!strcmp(s, "recovery"))
		mfg_mode = 2;
	else if (!strcmp(s, "charge"))
		mfg_mode = 3;

	return 1;
}
__setup("androidboot.mode=", board_mfg_mode_init);


int board_mfg_mode(void)
{
	return mfg_mode;
}

/*
 * This is a hack to allow nduid to pass
 * as the serial number in Android.
 */
extern int nduid_id_setup(char *str);

static int __init board_serialno_setup(char *str)
{
	nduid_id_setup(str);
	return 0;
}
__setup("androidboot.serialno=", board_serialno_setup);

