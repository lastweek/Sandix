/*
 * Low Level VGA-Based Console Driver.
 * Many thanks to Linux code authors.
 */

#include <sandix/types.h>
#include <sandix/bootparam.h>
#include <sandix/screen_info.h>
#include <video/vga.h>

#define SCREEN_START 0xB8000
#define SCREEN_END   0xC0000
#define ATTRIBUTE    0x07
#define LINES	24
#define COLUMNS	80
#define NPAR	16

static unsigned char attr = ATTRIBUTE;
static size_t top = 0, bottom = LINES;
static size_t lines = LINES, columns = COLUMNS;
static size_t scr_start_addr = SCREEN_START;
static size_t scr_end_addr   = SCREEN_END;
static size_t x, y;  /* Range: x~[0,79], y~[0,23] */
static size_t pos;   /* Current cursor position   */
static size_t state; /* Control console behaviour */

static inline void write_vga(unsigned char reg, unsigned int val)
{
	outb_p(reg, vga_video_port_reg);
	outb_p(val >> 8, vga_video_port_val);
	outb_p(reg + 1, vga_video_port_reg);
	outb_p(val & 0xff, vga_video_port_val);
}


/*
 * Start Address field specifies the display memory 
 * address of the upper left character of the screen.
 * Start Address High Register (Index 0Ch)
 * Start Address Low  Register (Index 0Dh)
 */
static inline void
vga_set_mem_top(struct vc_data *c)
{
	write_vga(12, (c->vc_visible_origin - vga_vram_base) / 2);
}

static void
set_start_addr(void)
{
	outb(0x0C, 0x3D4);
	outb(0xFF & ((scr_start_addr - SCREEN_START) >> 9), 0x3D5);
	outb(0x0D, 0x3D4);
	outb(0xFF & ((scr_start_addr - SCREEN_START) >> 1), 0x3D5);
}

static int
vgacon_set_origin(struct vc_data *c)
{
	if (vga_is_gfx || (console_blanked && !vga_palette_blanked))
		return 0;
	c->vc_origin = c->vc_visible_origin = vga_vram_base;
	vga_set_mem_top(c);
	vga_rolled_over = 0;
	return 1;
}

/*
 * Cursor Location field specifies the memory
 * address of the current cursor in screen.
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


static void
gotoxy(size_t new_x, size_t new_y)
{
	if (new_x >= columns || new_y >= lines)
		return;

	x = new_x;
	y = new_y;
	pos = scr_start_addr + ((x+y*columns)<<1);
}

static void
screen_up(void)
{
	
}

static void
screen_down(void)
{
	
}

static void lf(void)


/* Delete on current position */
static void delete(void)
{
	if (x) {
		*(__u8)pos = 0x20; /* space */
		*(__u8)(pos+1) = attr;
		pos -= 2;
		x--;
	}
}

static int saved_x=0;
static int saved_y=0;

static void save_cur(void)
{
	saved_x = x;
	saved_y = y;
}

static void restore_cur(void)
{
	x = saved_x;
	y = saved_y;
	pos = scr_start_addr + ((x+y*columns)<<1);
}

void vgacon_write(char *s)
{
	
}

void vgacon_init(void)
{
	unsigned char a;

	gotoxy(*(unsigned char *)(0x90000+510),*(unsigned char *)(0x90000+511));
	set_trap_gate(0x21,&keyboard_interrupt);
	outb_p(inb_p(0x21)&0xfd,0x21);
	a=inb_p(0x61);
	outb_p(a|0x80,0x61);
	outb(a,0x61);
}

void vgacon_deinit(void)
{
	
}
