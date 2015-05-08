/*
 *	2015/05/07 Created by Shan Yizhou.
 *
 *	console.c: "TTY Driver" for Console IO.
 *
 *	Note about Video Card:
 *	The CRT Controller (CRTC) Registers are accessed via a pair of 
 *	registers, the CRTC Index Register and the CRTC Data Register.
 *	The Index Register is located at port 3D4h. The Data Register
 *	is located at port 3D5h.
 */

#include <sandix/types.h>
#include <asm/io.h>

#define SCREEN_START 0xB8000
#define SCREEN_END   0xC0000
#define ATTRIBUTE    0x07
#define LINES	24
#define COLUMNS	80

static __u8   attr    = ATTRIBUTE;
static size_t lines   = LINES;
static size_t columns = COLUMNS;
static size_t scr_start_addr = SCREEN_START;
static size_t scr_end_addr   = SCREEN_END;
static size_t x, y;  /* Range: x~[0,79], y~[0,23] */
static size_t pos;   /* Current cursor position   */
static size_t state; /* Control console behaviour */

/*
 * Cursor Location field specifies the memory
 * address of the current cursor in screen.
 *
 * Cursor Location High 8-bit Register (Index 0Eh)
 * Cursor Location Low  8-bit Register (Index 0Fh)
 */
static void set_cursor(void)
{
	outb(0x0E, 0x3D4);
	outb(0xFF & ((pos - SCREEN_START) >> 9), 0x3D5);
	outb(0x0F, 0x3D4);
	outb(0xFF & ((pos - SCREEN_START) >> 1), 0x3D5);
}

/*
 * Start Address field specifies the display memory 
 * address of the upper left character of the screen.
 *
 * Start Address High Register (Index 0Ch)
 * Start Address Low  Register (Index 0Dh)
 */
static void set_start_addr(void)
{
	outb(0x0C, 0x3D4);
	outb(0xFF & ((scr_start_addr - SCREEN_START) >> 9), 0x3D5);
	outb(0x0D, 0x3D4);
	outb(0xFF & ((scr_start_addr - SCREEN_START) >> 1), 0x3D5);
}

static void gotoxy(size_t new_x, size_t new_y)
{
	if (new_x >= columns || new_y >= lines)
		return;

	x = new_x;
	y = new_y;
	pos = scr_start_addr + ((x + y*columns) << 1);
}

static void delete(void)
{
	if (x) {
		*(__u8)pos = 0x20; /* space */
		*(__u8)(pos+1) = attr;
		pos -= 2;
		x--;
	}
}




/* FIXME */
void con_write(char *s)
{

}

/* FIXME */
void con_init()
{
	
}
