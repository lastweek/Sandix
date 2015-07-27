#include <sandix/types.h>
#include <sandix/console.h>

#define DUMMY_COLUMNS	80
#define DUMMY_ROWS	25

static const char *dummycon_startup(void)
{
    return "dummy device";
}

static void dummycon_init(struct vc_data *vc, int init)
{
	vc->vc_cols = DUMMY_COLUMNS;
	vc->vc_rows = DUMMY_ROWS;
}

static int dummycon_dummy(void)
{
    return 0;
}

#define DUMMY	(void *)dummycon_dummy

const struct consw dummy_con = {
    .con_startup =	dummycon_startup,
    .con_init =		dummycon_init,
    .con_deinit =	DUMMY,
    .con_clear =	DUMMY,
    .con_putc =		DUMMY,
    .con_putcs =	DUMMY,
    .con_cursor =	DUMMY,
    .con_scroll =	DUMMY,
    .con_bmove =	DUMMY,
    .con_switch =	DUMMY,
    .con_blank =	DUMMY,
    .con_font_set =	DUMMY,
    .con_font_get =	DUMMY,
    .con_font_default =	DUMMY,
    .con_font_copy =	DUMMY,
    .con_set_palette =	DUMMY,
    .con_scrolldelta =	DUMMY,
};
