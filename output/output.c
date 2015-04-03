/*
 *	Output to screen.
 *	2015/04/02 Created by Shan Yizhou.
 *
 *	80*25 mode, each character has one byte for attribute,
 *	one for itself. Mapped region start address: 0xB800:0000.
 */

#include "output.h"

#define SC_MAX_ROW	80
#define SC_MAX_COL	25
#define SC_MAX_PAGE	4

char *max_map_addr  = 0x000B8F9F;	// upper address of mapped mempry.
char *base_map_addr = 0x000B8000;	// base address of mapped memory.
char *sc_addr_cur;	// current cursor address.
char  sc_attr_cur;	// current output attribute.
int   sc_row_cur, sc_col_cur;	// current cursor position, range: 1--80, 1--25

void screen_init() {
	sc_row_cur = 0;
	sc_col_cur = 0;
	sc_addr_cur = base_map_addr;
	sc_attr_cur = 0x12;
}

// Show a character on screen.
static void screenc(char c) {
	int offset;
	offset = (sc_row_cur-1) * SC_MAX_ROW + sc_col_cur;
	sc_addr_cur = (offset-1) * 2 + base_map_addr;

	*sc_addr_cur++ = c;
	*sc_addr_cur++ = sc_attr_cur;

	sc_row_cur++;
	sc_col_cur++;
}


