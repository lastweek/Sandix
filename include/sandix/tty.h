/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shan13@purdue.edu>
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

#include <sandix/fs.h>
#include <sandix/kref.h>
#include <sandix/types.h>
#include <sandix/rwsem.h>
#include <sandix/mutex.h>
#include <sandix/termios.h>
#include <sandix/compiler.h>
#include <sandix/spinlock.h>
#include <sandix/tty_ldisc.h>
#include <sandix/tty_driver.h>

/*
 *	XXX
 *	All functions that manipulate locks should be reviewed
 *	when Sandix is going to run on real SMP machines.
 */
struct tty_struct {
	int				magic;
	int				index;
	unsigned long			flags;

	spinlock_t			flow_lock;
	int				stopped;

	/* protect whole tty struct */
	struct mutex			tty_lock;
	struct kref			kref;

	struct rw_semaphore		termios_rwsem;
	struct termios			termios;

	struct tty_driver		*driver;
	const struct tty_operations	*ops;
	void 				*driver_data;

	struct rw_semaphore		ldisc_rwsem;
	struct tty_ldisc 		*ldisc;
	void 				*ldisc_data;

#define TTY_WRITE_BUF_SIZE 2048
	int				write_cnt;
	unsigned char 			write_buf[TTY_WRITE_BUF_SIZE];
};

/*
 * These bits are used in the flags field of the tty structure.
 *
 * So that interrupts won't be able to mess up the queues,
 * copy_to_cooked must be atomic with respect to itself, as must
 * tty->write.  Thus, you must use the inline functions set_bit() and
 * clear_bit() to make things atomic.
 */
#define TTY_THROTTLED 		0	/* Call unthrottle() at threshold min */
#define TTY_IO_ERROR 		1	/* Cause an I/O error (may be no ldisc too) */
#define TTY_OTHER_CLOSED 	2	/* Other side (if any) has closed */
#define TTY_EXCLUSIVE 		3	/* Exclusive open mode */
#define TTY_DEBUG 		4	/* Debugging */
#define TTY_DO_WRITE_WAKEUP 	5	/* Call write_wakeup after queuing new */
#define TTY_OTHER_DONE		6	/* Closed pty has completed input processing */
#define TTY_LDISC_OPEN	 	11	/* Line discipline is open */
#define TTY_PTY_LOCK 		16	/* pty private */
#define TTY_NO_WRITE_SPLIT 	17	/* Preserve write boundaries to driver */
#define TTY_HUPPED 		18	/* Post driver->hangup() */
#define TTY_LDISC_HALTED	22	/* Line discipline is halted */

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

/* XXX This should not be exported. Delete this after 
       char device layer and filesystem operation stuff are built */
ssize_t tty_write(struct file *file, const char __user *buf, size_t count, loff_t **ppos);


extern struct tty_ldisc_ops tty_ldisc_N_TTY;
extern struct termios tty_std_termios;
extern struct list_head tty_drivers;

void tty_set_operations(struct tty_driver *driver, const struct tty_operations *ops);
int tty_unregister_driver(struct tty_driver *driver);
int tty_register_driver(struct tty_driver *driver);
void tty_print_drivers(void);
void tty_lock(struct tty_struct *tty);
void tty_unlock(struct tty_struct *tty);
struct tty_struct *alloc_tty_struct(struct tty_driver *driver, int idx);
int tty_chars_in_buffer(struct tty_struct *tty);
int tty_write_room(struct tty_struct *tty);
void tty_driver_flush_buffer(struct tty_struct *tty);
int tty_put_char(struct tty_struct *tty, unsigned char ch);
void __init tty_init(void);

#endif /* _SANDIX_TTY_H_ */
