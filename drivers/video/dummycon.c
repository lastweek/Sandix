#include <sandix/console.h>

#define DUMMY_COLUMNS	80
#define DUMMY_ROWS	25

static void dummycon_startup(void)
{
	return;
}

static void dummycon_init(struct vc_struct *vc)
{
	vc->vc_cols = DUMMY_COLUMNS;
	vc->vc_rows = DUMMY_ROWS;
}

static int dummycon_dummy(void)
{
    return 0;
}

#define DUMMY	(void *)dummycon_dummy

const struct con_driver dummy_con = {
    .con_startup	=	dummycon_startup,
    .con_init		=	dummycon_init,
    .con_deinit		=	DUMMY,
    .con_clear		=	DUMMY,
    .con_putc		=	DUMMY,
    .con_putcs		=	DUMMY,
    .con_cursor		=	DUMMY,
    .con_scroll		=	DUMMY,
    .con_scrolldelta	=	DUMMY,
};
