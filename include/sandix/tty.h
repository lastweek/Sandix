/*
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * README:
 * The tty core is responsible for controlling both the flow of data across a
 * tty device and the format of the data. This allows tty drivers to focus on
 * handling the data to and from the *hardware*, instead of worrying about how
 * to control the interaction with user space in a consistent way. To control
 * the flow of data, there are a number of different line disciplines that can
 * be virtually "plugged" into any tty device. This is done by different tty
 * line discipline drivers.
 *
 * There are three different types of tty drivers: Console, Serial port and PTY.
 * Console drivers communicate with the screen and input keyboard. Serial port
 * drivers communicate with the serial port such as RS232. PTY drivers consists
 * of master and slave part.
 *
 * In Sandix, there is only one tty driver available: Console drivers. This tty
 * driver controls the screen output and receives characters from keyboard. Just
 * a note, the underlying screen output also has an additional abstraction. Since
 * we may have VGA screen or MDA screen or others. That is why we have driver_data
 * field in tty_struct: Make tty drivers more general, to have another abstraction
 * if they want to.
 */

#ifndef _SANDIX_TTY_H_
#define _SANDIX_TTY_H_

#include <sandix/compiler.h>
#include <sandix/termios.h>
#include <sandix/types.h>

struct tty_struct;

/* struct tty_operations
 * @write:		Write to tty device
 * @put_char:		Write one character to tty device
 *
 * This structure defines the interface between the low-level tty driver and
 * the tty routines. The following routines can be defined; unless noted
 * otherwise, they are optional, and can be filled in with a null pointer.
 */
struct tty_operations {
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, int count);
	int	(*put_char)(struct tty_struct *tty, unsigned char ch);
};

/**
 * struct tty_driver
 * @name:		Nick name
 * @type:		Type of this tty driver
 * @major:		Major number of this tty driver
 * @minor_start:	Starting minor number of this tty driver
 * @num:		Number of devices allocated
 * @inint_termios:	Termios of this driver
 * @ops:		Hardware-operations of this tty driver
 * @tty_drivers:	Linked list of all registed tty driver
 *
 * The driver's job is to format data that is sent to it in a manner that the
 * hardware can understand, and receive data from the hardware.
 */
struct tty_driver {
	const char	*name;		
	unsigned int	type;		
	unsigned int	major;		
	unsigned int	minor_start;	
	unsigned int	num;		
	struct termios	init_termios;
	const struct tty_operations *ops;
	struct list_head tty_drivers;
};

/**
 * struct tty_ldisc_ops
 * @read:		Read method
 * @write:		Write method
 * @set_termios:	Replace termios
 *
 * This structure defines the interface between the tty line discipline
 * implementation and the tty routines. The following routines can be
 * defined; unless noted otherwise, they are optional, and can be filled
 * in with a null pointer.
 */
struct tty_ldisc_ops {
	int	(*read)(struct tty_struct *tty, unsigned char __user *buf, size_t len);
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, size_t len);
	void	(*set_termios)(struct tty_struct *tty, struct termios *old);
};

/**
 * struct tty_ldisc
 * @ops:		Line discipline methods
 * @tty:		tty_struct who own this line discipline
 *
 * The tty line discipline's job is to format the data received from a user,
 * or the hardware, in a specific manner. This formatting usually takes the
 * form of a protocol conversion, such as PPP or Bluetooth.(?)
 */
struct tty_ldisc {
	struct tty_ldisc_ops *ops;
	struct tty_struct *tty;
};

/**
 * struct tty_struct
 * @magic:		Magic number for all tty structs
 * @termios:		Termios for this tty
 * @ldisc:		Line discipline driver for this tty
 * @driver:		Low-Level tty driver for this tty
 * @ops:		Hardware-operations of tty driver
 * @disc_data:		Additional data used by line discipline driver
 * @driver_data:	Additional data used by tty driver
 */
struct tty_struct {
	unsigned int magic;
	struct termios termios;
	struct tty_ldisc *ldisc;
	struct tty_driver *driver;
	const struct tty_operations *ops;
	void *disc_data;
	void *driver_data;
};

/* TTY Driver Types */
#define TTY_DRIVER_TYPE_SYSTEM	0x0001
#define TTY_DRIVER_TYPE_CONSOLE	0x0002
#define TTY_DRIVER_TYPE_SERIAL	0x0003
#define TTY_DRIVER_TYPE_PTY	0x0004
#define TTY_DRIVER_TYPE_DUMMY	0x0005

/* Line Disciplines */
#define N_TTY			0
#define N_SLIP			1
#define N_MOUSE			2
#define N_PPP			3
#define N_STRIP			4
#define N_AX25			5
#define N_X25			6	/* X.25 async */
#define N_6PACK			7
#define N_MASC			8	/* Reserved for Mobitex module <kaz@cafe.net> */
#define N_R3964			9	/* Reserved for Simatic R3964 module */
#define N_PROFIBUS_FDL		10	/* Reserved for Profibus */
#define N_IRDA			11	/* Linux IrDa - http://irda.sourceforge.net/ */
#define N_SMSBLOCK		12	/* SMS block mode - for talking to GSM data */
#define N_HDLC			13	/* synchronous HDLC */
#define N_SYNC_PPP		14	/* synchronous PPP */
#define N_HCI			15	/* Bluetooth HCI UART */
#define N_GIGASET_M101		16	/* Siemens Gigaset M101 serial DECT adapter */
#define N_SLCAN			17	/* Serial / USB serial CAN Adaptors */
#define N_PPS			18	/* Pulse per Second */
#define N_V253			19	/* Codec control over voice modem */
#define N_CAIF			20      /* CAIF protocol for talking to modems */
#define N_GSM0710		21	/* GSM 0710 Mux */
#define N_TI_WL			22	/* for TI's WL BT, FM, GPS combo chips */
#define N_TRACESINK		23	/* Trace data routing for MIPI P1149.7 */
#define N_TRACEROUTER		24	/* Trace data routing for MIPI P1149.7 */
#define N_NCI			25	/* NFC NCI UART */
#define NR_LDISCS		30

#define INTR_CHAR(tty)		((tty)->termios.c_cc[VINTR])
#define QUIT_CHAR(tty)		((tty)->termios.c_cc[VQUIT])
#define ERASE_CHAR(tty)		((tty)->termios.c_cc[VERASE])
#define KILL_CHAR(tty)		((tty)->termios.c_cc[VKILL])
#define EOF_CHAR(tty)		((tty)->termios.c_cc[VEOF])
#define TIME_CHAR(tty)		((tty)->termios.c_cc[VTIME])
#define MIN_CHAR(tty)		((tty)->termios.c_cc[VMIN])
#define SWTC_CHAR(tty)		((tty)->termios.c_cc[VSWTC])
#define START_CHAR(tty)		((tty)->termios.c_cc[VSTART])
#define STOP_CHAR(tty)		((tty)->termios.c_cc[VSTOP])
#define SUSP_CHAR(tty)		((tty)->termios.c_cc[VSUSP])
#define EOL_CHAR(tty)		((tty)->termios.c_cc[VEOL])
#define REPRINT_CHAR(tty)	((tty)->termios.c_cc[VREPRINT])
#define DISCARD_CHAR(tty)	((tty)->termios.c_cc[VDISCARD])
#define WERASE_CHAR(tty)	((tty)->termios.c_cc[VWERASE])
#define LNEXT_CHAR(tty)		((tty)->termios.c_cc[VLNEXT])
#define EOL2_CHAR(tty)		((tty)->termios.c_cc[VEOL2])

#define _I_FLAG(tty, f)		((tty)->termios.c_iflag & (f))
#define _O_FLAG(tty, f)		((tty)->termios.c_oflag & (f))
#define _C_FLAG(tty, f)		((tty)->termios.c_cflag & (f))
#define _L_FLAG(tty, f)		((tty)->termios.c_lflag & (f))

#define I_IGNBRK(tty)		_I_FLAG((tty), IGNBRK)
#define I_BRKINT(tty)		_I_FLAG((tty), BRKINT)
#define I_IGNPAR(tty)		_I_FLAG((tty), IGNPAR)
#define I_PARMRK(tty)		_I_FLAG((tty), PARMRK)
#define I_INPCK(tty)		_I_FLAG((tty), INPCK)
#define I_ISTRIP(tty)		_I_FLAG((tty), ISTRIP)
#define I_INLCR(tty)		_I_FLAG((tty), INLCR)
#define I_IGNCR(tty)		_I_FLAG((tty), IGNCR)
#define I_ICRNL(tty)		_I_FLAG((tty), ICRNL)
#define I_IUCLC(tty)		_I_FLAG((tty), IUCLC)
#define I_IXON(tty)		_I_FLAG((tty), IXON)
#define I_IXANY(tty)		_I_FLAG((tty), IXANY)
#define I_IXOFF(tty)		_I_FLAG((tty), IXOFF)
#define I_IMAXBEL(tty)		_I_FLAG((tty), IMAXBEL)
#define I_IUTF8(tty)		_I_FLAG((tty), IUTF8)

#define O_OPOST(tty)		_O_FLAG((tty), OPOST)
#define O_OLCUC(tty)		_O_FLAG((tty), OLCUC)
#define O_ONLCR(tty)		_O_FLAG((tty), ONLCR)
#define O_OCRNL(tty)		_O_FLAG((tty), OCRNL)
#define O_ONOCR(tty)		_O_FLAG((tty), ONOCR)
#define O_ONLRET(tty)		_O_FLAG((tty), ONLRET)
#define O_OFILL(tty)		_O_FLAG((tty), OFILL)
#define O_OFDEL(tty)		_O_FLAG((tty), OFDEL)
#define O_NLDLY(tty)		_O_FLAG((tty), NLDLY)
#define O_CRDLY(tty)		_O_FLAG((tty), CRDLY)
#define O_TABDLY(tty)		_O_FLAG((tty), TABDLY)
#define O_BSDLY(tty)		_O_FLAG((tty), BSDLY)
#define O_VTDLY(tty)		_O_FLAG((tty), VTDLY)
#define O_FFDLY(tty)		_O_FLAG((tty), FFDLY)

#define C_BAUD(tty)		_C_FLAG((tty), CBAUD)
#define C_CSIZE(tty)		_C_FLAG((tty), CSIZE)
#define C_CSTOPB(tty)		_C_FLAG((tty), CSTOPB)
#define C_CREAD(tty)		_C_FLAG((tty), CREAD)
#define C_PARENB(tty)		_C_FLAG((tty), PARENB)
#define C_PARODD(tty)		_C_FLAG((tty), PARODD)
#define C_HUPCL(tty)		_C_FLAG((tty), HUPCL)
#define C_CLOCAL(tty)		_C_FLAG((tty), CLOCAL)
#define C_CIBAUD(tty)		_C_FLAG((tty), CIBAUD)
#define C_CRTSCTS(tty)		_C_FLAG((tty), CRTSCTS)
#define C_CMSPAR(tty)		_C_FLAG((tty), CMSPAR)

#define L_ISIG(tty)		_L_FLAG((tty), ISIG)
#define L_ICANON(tty)		_L_FLAG((tty), ICANON)
#define L_XCASE(tty)		_L_FLAG((tty), XCASE)
#define L_ECHO(tty)		_L_FLAG((tty), ECHO)
#define L_ECHOE(tty)		_L_FLAG((tty), ECHOE)
#define L_ECHOK(tty)		_L_FLAG((tty), ECHOK)
#define L_ECHONL(tty)		_L_FLAG((tty), ECHONL)
#define L_NOFLSH(tty)		_L_FLAG((tty), NOFLSH)
#define L_TOSTOP(tty)		_L_FLAG((tty), TOSTOP)
#define L_ECHOCTL(tty)		_L_FLAG((tty), ECHOCTL)
#define L_ECHOPRT(tty)		_L_FLAG((tty), ECHOPRT)
#define L_ECHOKE(tty)		_L_FLAG((tty), ECHOKE)
#define L_FLUSHO(tty)		_L_FLAG((tty), FLUSHO)
#define L_PENDIN(tty)		_L_FLAG((tty), PENDIN)
#define L_IEXTEN(tty)		_L_FLAG((tty), IEXTEN)
#define L_EXTPROC(tty)		_L_FLAG((tty), EXTPROC)

/* FIXME */
/* tty_table[0] is registed as console tty */
extern struct tty_struct tty_table[2];

extern struct termios tty_std_termios;

void tty_set_operations(struct tty_driver *driver, const struct tty_operations *ops);
int tty_unregister_driver(struct tty_driver *driver);
int tty_register_driver(struct tty_driver *driver);
struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx);

void __init tty_init(void);

#endif /* _SANDIX_TTY_H_ */
