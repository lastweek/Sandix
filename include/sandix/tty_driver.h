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

#ifndef _SANDIX_TTY_DRIVE_H_
#define _SANDIX_TTY_DRIVE_H_

#ifndef _SANDIX_TTY_H_
# error "Please do not include this file directly"
#endif

struct tty_struct;

/*
 * This structure defines the interface between the low-level tty
 * driver and the tty routines.  The following routines can be
 * defined; unless noted otherwise, they are optional, and can be
 * filled in with a null pointer.
 *
 * struct tty_struct * (*lookup)(struct tty_driver *self, int idx)
 *
 *	Return the tty device corresponding to idx, NULL if there is not
 *	one currently in use and an ERR_PTR value on error. Called under
 *	tty_mutex (for now!)
 *
 *	Optional method. Default behaviour is to use the ttys array
 *
 * int (*install)(struct tty_driver *self, struct tty_struct *tty)
 *
 *	Install a new tty into the tty driver internal tables. Used in
 *	conjunction with lookup and remove methods.
 *
 *	Optional method. Default behaviour is to use the ttys array
 *
 * void (*remove)(struct tty_driver *self, struct tty_struct *tty)
 *
 *	Remove a closed tty from the tty driver internal tables. Used in
 *	conjunction with lookup and remove methods.
 *
 *	Optional method. Default behaviour is to use the ttys array
 *
 * int  (*open)(struct tty_struct * tty, struct file * filp);
 *
 * 	This routine is called when a particular tty device is opened.
 * 	This routine is mandatory; if this routine is not filled in,
 * 	the attempted open will fail with ENODEV.
 *
 *	Required method. Called with tty lock held.
 *
 * void (*close)(struct tty_struct * tty, struct file * filp);
 *
 * 	This routine is called when a particular tty device is closed.
 *	Note: called even if the corresponding open() failed.
 *
 *	Required method. Called with tty lock held.
 *
 * void (*shutdown)(struct tty_struct * tty);
 *
 * 	This routine is called under the tty lock when a particular tty device
 *	is closed for the last time. It executes before the tty resources
 *	are freed so may execute while another function holds a tty kref.
 *
 * void (*cleanup)(struct tty_struct * tty);
 *
 *	This routine is called asynchronously when a particular tty device
 *	is closed for the last time freeing up the resources. This is
 *	actually the second part of shutdown for routines that might sleep.
 *
 *
 * int (*write)(struct tty_struct * tty,
 * 		 const unsigned char *buf, int count);
 *
 * 	This routine is called by the kernel to write a series of
 * 	characters to the tty device.  The characters may come from
 * 	user space or kernel space.  This routine will return the
 *	number of characters actually accepted for writing.
 *
 *	Optional: Required for writable devices.
 *
 * int (*put_char)(struct tty_struct *tty, unsigned char ch);
 *
 * 	This routine is called by the kernel to write a single
 * 	character to the tty device.  If the kernel uses this routine,
 * 	it must call the flush_chars() routine (if defined) when it is
 * 	done stuffing characters into the driver.  If there is no room
 * 	in the queue, the character is ignored.
 *
 *	Optional: Kernel will use the write method if not provided.
 *
 *	Note: Do not call this function directly, call tty_put_char
 *
 * void (*flush_chars)(struct tty_struct *tty);
 *
 * 	This routine is called by the kernel after it has written a
 * 	series of characters to the tty device using put_char().  
 *
 *	Optional:
 *
 *	Note: Do not call this function directly, call tty_driver_flush_chars
 * 
 * int  (*write_room)(struct tty_struct *tty);
 *
 * 	This routine returns the numbers of characters the tty driver
 * 	will accept for queuing to be written.  This number is subject
 * 	to change as output buffers get emptied, or if the output flow
 *	control is acted.
 *
 *	Required if write method is provided else not needed.
 *
 *	Note: Do not call this function directly, call tty_write_room
 * 
 * int  (*ioctl)(struct tty_struct *tty, unsigned int cmd, unsigned long arg);
 *
 * 	This routine allows the tty driver to implement
 *	device-specific ioctls.  If the ioctl number passed in cmd
 * 	is not recognized by the driver, it should return ENOIOCTLCMD.
 *
 *	Optional
 *
 * long (*compat_ioctl)(struct tty_struct *tty,,
 * 	                unsigned int cmd, unsigned long arg);
 *
 * 	implement ioctl processing for 32 bit process on 64 bit system
 *
 *	Optional
 * 
 * void (*set_termios)(struct tty_struct *tty, struct ktermios * old);
 *
 * 	This routine allows the tty driver to be notified when
 * 	device's termios settings have changed.
 *
 *	Optional: Called under the termios lock
 *
 *
 * void (*set_ldisc)(struct tty_struct *tty);
 *
 * 	This routine allows the tty driver to be notified when the
 * 	device's termios settings have changed.
 *
 *	Optional: Called under BKL (currently)
 * 
 * void (*throttle)(struct tty_struct * tty);
 *
 * 	This routine notifies the tty driver that input buffers for
 * 	the line discipline are close to full, and it should somehow
 * 	signal that no more characters should be sent to the tty.
 *
 *	Optional: Always invoke via tty_throttle(), called under the
 *	termios lock.
 * 
 * void (*unthrottle)(struct tty_struct * tty);
 *
 * 	This routine notifies the tty drivers that it should signals
 * 	that characters can now be sent to the tty without fear of
 * 	overrunning the input buffers of the line disciplines.
 * 
 *	Optional: Always invoke via tty_unthrottle(), called under the
 *	termios lock.
 *
 * void (*stop)(struct tty_struct *tty);
 *
 * 	This routine notifies the tty driver that it should stop
 * 	outputting characters to the tty device.  
 *
 *	Called with ->flow_lock held. Serialized with start() method.
 *
 *	Optional:
 *
 *	Note: Call stop_tty not this method.
 * 
 * void (*start)(struct tty_struct *tty);
 *
 * 	This routine notifies the tty driver that it resume sending
 *	characters to the tty device.
 *
 *	Called with ->flow_lock held. Serialized with stop() method.
 *
 *	Optional:
 *
 *	Note: Call start_tty not this method.
 * 
 * void (*hangup)(struct tty_struct *tty);
 *
 * 	This routine notifies the tty driver that it should hang up the
 * 	tty device.
 *
 *	Optional:
 *
 *	Called with tty lock held.
 *
 * int (*break_ctl)(struct tty_struct *tty, int state);
 *
 * 	This optional routine requests the tty driver to turn on or
 * 	off BREAK status on the RS-232 port.  If state is -1,
 * 	then the BREAK status should be turned on; if state is 0, then
 * 	BREAK should be turned off.
 *
 * 	If this routine is implemented, the high-level tty driver will
 * 	handle the following ioctls: TCSBRK, TCSBRKP, TIOCSBRK,
 * 	TIOCCBRK.
 *
 *	If the driver sets TTY_DRIVER_HARDWARE_BREAK then the interface
 *	will also be called with actual times and the hardware is expected
 *	to do the delay work itself. 0 and -1 are still used for on/off.
 *
 *	Optional: Required for TCSBRK/BRKP/etc handling.
 *
 * void (*wait_until_sent)(struct tty_struct *tty, int timeout);
 * 
 * 	This routine waits until the device has written out all of the
 * 	characters in its transmitter FIFO.
 *
 *	Optional: If not provided the device is assumed to have no FIFO
 *
 *	Note: Usually correct to call tty_wait_until_sent
 *
 * void (*send_xchar)(struct tty_struct *tty, char ch);
 *
 * 	This routine is used to send a high-priority XON/XOFF
 * 	character to the device.
 *
 *	Optional: If not provided then the write method is called under
 *	the atomic write lock to keep it serialized with the ldisc.
 *
 * int (*resize)(struct tty_struct *tty, struct winsize *ws)
 *
 *	Called when a termios request is issued which changes the
 *	requested terminal geometry.
 *
 *	Optional: the default action is to update the termios structure
 *	without error. This is usually the correct behaviour. Drivers should
 *	not force errors here if they are not resizable objects (eg a serial
 *	line). See tty_do_resize() if you need to wrap the standard method
 *	in your own logic - the usual case.
 *
 * void (*set_termiox)(struct tty_struct *tty, struct termiox *new);
 *
 *	Called when the device receives a termiox based ioctl. Passes down
 *	the requested data from user space. This method will not be invoked
 *	unless the tty also has a valid tty->termiox pointer.
 *
 *	Optional: Called under the termios lock
 *
 * int (*get_icount)(struct tty_struct *tty, struct serial_icounter *icount);
 *
 *	Called when the device receives a TIOCGICOUNT ioctl. Passed a kernel
 *	structure to complete. This method is optional and will only be called
 *	if provided (otherwise EINVAL will be returned).
 */

struct tty_operations {
	int	(*write)(struct tty_struct *tty, const unsigned char *buf, int count);
	int	(*put_char)(struct tty_struct *tty, unsigned char ch);
	int	(*write_room)(struct tty_struct *tty);
	int	(*chars_in_buffer)(struct tty_struct *tty);
	void	(*flush_chars)(struct tty_struct *tty);
	void	(*flush_buffer)(struct tty_struct *tty);
};

/**
 * struct tty_driver
 * @name:		Nick name
 * @flags:		flags of tty driver
 * @type:		Type of this tty driver(See below)
 * @major:		Major number of this tty driver
 * @minor_start:	Starting minor number of this tty driver
 * @num:		Number of devices allocated
 * @init_termios:	Termios of this driver
 * @kref:		Reference count
 * @tty_drivers:	Linked list of all registed tty driver
 * @ops:		Hardware-operations of this tty driver
 *
 * The driver's job is to format data that is sent to it in a manner that the
 * hardware can understand, and receive data from the hardware.
 */
struct tty_driver {
	const char	*name;		
	unsigned long	flags;
	unsigned int	type;		
	unsigned int	major;		
	unsigned int	minor_start;	
	unsigned int	num;		
	struct termios	init_termios;
	struct kref	kref;

	/*
	 * Driver methods
	 */
	const struct tty_operations *ops;
	struct list_head tty_drivers;
};

/*
 * tty driver flags
 * 
 * TTY_DRIVER_RESET_TERMIOS --- requests the tty layer to reset the
 * 	termios setting when the last process has closed the device.
 * 	Used for PTY's, in particular.
 * 
 * TTY_DRIVER_REAL_RAW --- if set, indicates that the driver will
 * 	guarantee never not to set any special character handling
 * 	flags if ((IGNBRK || (!BRKINT && !PARMRK)) && (IGNPAR ||
 * 	!INPCK)).  That is, if there is no reason for the driver to
 * 	send notifications of parity and break characters up to the
 * 	line driver, it won't do so.  This allows the line driver to
 *	optimize for this case if this flag is set.  (Note that there
 * 	is also a promise, if the above case is true, not to signal
 * 	overruns, either.)
 *
 * TTY_DRIVER_DYNAMIC_DEV --- if set, the individual tty devices need
 *	to be registered with a call to tty_register_device() when the
 *	device is found in the system and unregistered with a call to
 *	tty_unregister_device() so the devices will be show up
 *	properly in sysfs.  If not set, driver->num entries will be
 *	created by the tty core in sysfs when tty_register_driver() is
 *	called.  This is to be used by drivers that have tty devices
 *	that can appear and disappear while the main tty driver is
 *	registered with the tty core.
 *
 * TTY_DRIVER_DEVPTS_MEM -- don't use the standard arrays, instead
 *	use dynamic memory keyed through the devpts filesystem.  This
 *	is only applicable to the pty driver.
 *
 * TTY_DRIVER_HARDWARE_BREAK -- hardware handles break signals. Pass
 *	the requested timeout to the caller instead of using a simple
 *	on/off interface.
 *
 * TTY_DRIVER_DYNAMIC_ALLOC -- do not allocate structures which are
 *	needed per line for this driver as it would waste memory.
 *	The driver will take care.
 *
 * TTY_DRIVER_UNNUMBERED_NODE -- do not create numbered /dev nodes. In
 *	other words create /dev/ttyprintk and not /dev/ttyprintk0.
 *	Applicable only when a driver for a single tty device is
 *	being allocated.
 */
#define TTY_DRIVER_INSTALLED		0x0001
#define TTY_DRIVER_RESET_TERMIOS	0x0002
#define TTY_DRIVER_REAL_RAW		0x0004
#define TTY_DRIVER_DYNAMIC_DEV		0x0008
#define TTY_DRIVER_DEVPTS_MEM		0x0010
#define TTY_DRIVER_HARDWARE_BREAK	0x0020
#define TTY_DRIVER_DYNAMIC_ALLOC	0x0040
#define TTY_DRIVER_UNNUMBERED_NODE	0x0080

/*
 * tty driver types
 */
#define TTY_DRIVER_TYPE_SYSTEM	0x0001
#define TTY_DRIVER_TYPE_CONSOLE	0x0002
#define TTY_DRIVER_TYPE_SERIAL	0x0003
#define TTY_DRIVER_TYPE_PTY	0x0004
#define TTY_DRIVER_TYPE_DUMMY	0x0005

#endif /* _SANDIX_TTY_DRIVE_H_ */
