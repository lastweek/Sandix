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

#ifndef _SANDIX_TTY_LDISC_H_
#define _SANDIX_TTY_LDISC_H_

#ifndef _SANDIX_TTY_H_
# error "Please do not include this file directly"
#endif

struct tty_struct;

/*
 * This structure defines the interface between the tty line discipline
 * implementation and the tty routines.  The following routines can be
 * defined; unless noted otherwise, they are optional, and can be
 * filled in with a null pointer.
 *
 * int	(*open)(struct tty_struct *);
 *
 *	This function is called when the line discipline is associated
 *	with the tty.  The line discipline can use this as an
 *	opportunity to initialize any state needed by the ldisc routines.
 *
 * void	(*close)(struct tty_struct *);
 *
 *	This function is called when the line discipline is being
 *	shutdown, either because the tty is being closed or because
 *	the tty is being changed to use a new line discipline
 *
 * void	(*flush_buffer)(struct tty_struct *tty);
 *
 *	This function instructs the line discipline to clear its
 *	buffers of any input characters it may have queued to be
 *	delivered to the user mode process.
 *
 * ssize_t (*chars_in_buffer)(struct tty_struct *tty);
 *
 *	This function returns the number of input characters the line
 *	discipline may have queued up to be delivered to the user mode
 *	process.
 *
 * ssize_t (*read)(struct tty_struct * tty, struct file * file,
 *		   unsigned char * buf, size_t nr);
 *
 *	This function is called when the user requests to read from
 *	the tty.  The line discipline will return whatever characters
 *	it has buffered up for the user.  If this function is not
 *	defined, the user will receive an EIO error.
 *
 * ssize_t (*write)(struct tty_struct * tty, struct file * file,
 *		    const unsigned char * buf, size_t nr);
 *
 *	This function is called when the user requests to write to the
 *	tty.  The line discipline will deliver the characters to the
 *	low-level tty device for transmission, optionally performing
 *	some processing on the characters first.  If this function is
 *	not defined, the user will receive an EIO error.
 *
 * int	(*ioctl)(struct tty_struct * tty, struct file * file,
 *		 unsigned int cmd, unsigned long arg);
 *
 *	This function is called when the user requests an ioctl which
 *	is not handled by the tty layer or the low-level tty driver.
 *	It is intended for ioctls which affect line discpline
 *	operation.  Note that the search order for ioctls is (1) tty
 *	layer, (2) tty low-level driver, (3) line discpline.  So a
 *	low-level driver can "grab" an ioctl request before the line
 *	discpline has a chance to see it.
 *
 * long	(*compat_ioctl)(struct tty_struct * tty, struct file * file,
 *		        unsigned int cmd, unsigned long arg);
 *
 *	Process ioctl calls from 32-bit process on 64-bit system
 *
 * void	(*set_termios)(struct tty_struct *tty, struct ktermios * old);
 *
 *	This function notifies the line discpline that a change has
 *	been made to the termios structure.
 *
 * int	(*poll)(struct tty_struct * tty, struct file * file,
 *		  poll_table *wait);
 *
 *	This function is called when a user attempts to select/poll on a
 *	tty device.  It is solely the responsibility of the line
 *	discipline to handle poll requests.
 *
 * void	(*receive_buf)(struct tty_struct *, const unsigned char *cp,
 *		       char *fp, int count);
 *
 *	This function is called by the low-level tty driver to send
 *	characters received by the hardware to the line discpline for
 *	processing.  <cp> is a pointer to the buffer of input
 *	character received by the device.  <fp> is a pointer to a
 *	pointer of flag bytes which indicate whether a character was
 *	received with a parity error, etc. <fp> may be NULL to indicate
 *	all data received is TTY_NORMAL.
 *
 * void	(*write_wakeup)(struct tty_struct *);
 *
 *	This function is called by the low-level tty driver to signal
 *	that line discpline should try to send more characters to the
 *	low-level driver for transmission.  If the line discpline does
 *	not have any more data to send, it can just return. If the line
 *	discipline does have some data to send, please arise a tasklet
 *	or workqueue to do the real data transfer. Do not send data in
 *	this hook, it may leads to a deadlock.
 *
 * int (*hangup)(struct tty_struct *)
 *
 *	Called on a hangup. Tells the discipline that it should
 *	cease I/O to the tty driver. Can sleep. The driver should
 *	seek to perform this action quickly but should wait until
 *	any pending driver I/O is completed.
 *
 * void (*fasync)(struct tty_struct *, int on)
 *
 *	Notify line discipline when signal-driven I/O is enabled or
 *	disabled.
 *
 * void (*dcd_change)(struct tty_struct *tty, unsigned int status)
 *
 *	Tells the discipline that the DCD pin has changed its status.
 *	Used exclusively by the N_PPS (Pulse-Per-Second) line discipline.
 *
 * int	(*receive_buf2)(struct tty_struct *, const unsigned char *cp,
 *			char *fp, int count);
 *
 *	This function is called by the low-level tty driver to send
 *	characters received by the hardware to the line discpline for
 *	processing.  <cp> is a pointer to the buffer of input
 *	character received by the device.  <fp> is a pointer to a
 *	pointer of flag bytes which indicate whether a character was
 *	received with a parity error, etc. <fp> may be NULL to indicate
 *	all data received is TTY_NORMAL.
 *	If assigned, prefer this function for automatic flow control.
 */
struct tty_ldisc_ops {
	int		magic;
	const char	*name;
	int		num;
	int		flags;
	int		refcount;
	
	/*
	 * The following routines are called from above.
	 */
	int		(*open)(struct tty_struct *);
	void		(*close)(struct tty_struct *);
	void		(*flush_buffer)(struct tty_struct *tty);
	ssize_t		(*chars_in_buffer)(struct tty_struct *tty);
	ssize_t		(*read)(struct tty_struct *tty, struct file *file, unsigned char __user *buf, size_t count);
	ssize_t		(*write)(struct tty_struct *tty, struct file *file, const unsigned char *buf, size_t count);
	int		(*ioctl)(struct tty_struct *tty, struct file *file, unsigned int cmd, unsigned long arg);
	long		(*compat_ioctl)(struct tty_struct *tty, struct file *file, unsigned int cmd, unsigned long arg);
	void		(*set_termios)(struct tty_struct *tty, struct termios *old);
	unsigned int	(*poll)(struct tty_struct *, struct file *, void *);
	int		(*hangup)(struct tty_struct *tty);

	/*
	 * The following routines are called from below.
	 */
	void		(*receive_buf)(struct tty_struct *, const unsigned char *cp, char *fp, int count);
	void		(*write_wakeup)(struct tty_struct *);
	void		(*dcd_change)(struct tty_struct *, unsigned int);
	void		(*fasync)(struct tty_struct *tty, int on);
	int		(*receive_buf2)(struct tty_struct *, const unsigned char *cp, char *fp, int count);
};

/**
 * struct tty_ldisc
 *
 * @ops:		The line discipline operations
 * @tty:		The tty belongs to
 *
 * This structure is allocated dynamically when tty calls tty_set_ldisc(), etc.
 * In system's view, only the tty_ldisc_op is static. When tty changes its
 * line discipline, the old tty_ldisc would be freed, a new one malloced.
 * All in all, this structure is a bridge between up tty and down ops.
 */
struct tty_ldisc {
	struct tty_ldisc_ops *ops;
	struct tty_struct *tty;
};

/*
 * Line disciplines
 */
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

int tty_register_ldisc(int disc, struct tty_ldisc_ops *new_ldisc);
int tty_unregister_ldisc(int disc);
int tty_ldisc_open(struct tty_struct *tty, struct tty_ldisc *ld);
void tty_ldisc_close(struct tty_struct *tty, struct tty_ldisc *ld);
int tty_ldisc_change(struct tty_struct *tty, int disc);
void tty_ldisc_init(struct tty_struct *tty);
void tty_ldisc_deinit(struct tty_struct *tty);
struct tty_ldisc *tty_ldisc_ref_wait(struct tty_struct *tty);
void tty_ldisc_deref(struct tty_ldisc *ld);
void __init tty_ldisc_begin(void);

#endif /* _SANDIX_TTY_LDISC_H_ */
