/*
 *	Copyright (C) 2015-2016 Yizhou Shan <shanyizhou@ict.ac.cn>
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

struct tty_ldisc_ops {
	int		magic;
	const char	*name;
	int		num;
	int		flags;
	int		refcount;

	int	(*open)(struct tty_struct *tty);
	void	(*close)(struct tty_struct *tty);
	ssize_t	(*read)(struct tty_struct *tty, char __user *buf, size_t count);
	ssize_t	(*write)(struct tty_struct *tty, struct file *file, const unsigned char *buf, size_t count);
	void	(*set_termios)(struct tty_struct *tty, struct termios *old);
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

#endif /* _SANDIX_TTY_LDISC_H_ */
