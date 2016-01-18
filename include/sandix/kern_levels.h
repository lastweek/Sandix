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

#ifndef _SANDIX_KERN_LEVELS_H_
#define _SANDIX_KERN_LEVELS_H_

#define KERN_SOH	"\001"		/* ASCII Start Of Header */
#define KERN_SOH_ASCII	'\001'

#ifdef __non_exist__
#define KERN_EMERG	KERN_SOH "0"	/* System is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* Action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* Critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* Error conditions */
#define KERN_WARN	KERN_SOH "4"	/* Warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* Normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* Informational */
#define KERN_DEBUG	KERN_SOH "7"	/* Debug-level messages */
#define KERN_DEFAULT	KERN_SOH "d"	/* The default kernel loglevel */
#else
#define KERN_EMERG
#define KERN_ALERT
#define KERN_CRIT
#define KERN_ERR
#define KERN_WARN
#define KERN_NOTICE
#define KERN_INFO
#define KERN_DEBUG
#define KERN_DEFAULT
#endif

/*
 * Annotation for a "continued" line of log printout (only done after a
 * line that had no enclosing \n). Only to be used by core/arch code
 * during early bootup (a continued line is not SMP-safe otherwise).
 */
#define KERN_CONT	""

/* Integer equivalents of KERN_<LEVEL> */
#define LOGLEVEL_SCHED		-2	/* Deferred messages from sched code
					 * are set to this special level */
#define LOGLEVEL_DEFAULT	-1	/* Default (or last) loglevel */
#define LOGLEVEL_EMERG		0	/* System is unusable */
#define LOGLEVEL_ALERT		1	/* Action must be taken immediately */
#define LOGLEVEL_CRIT		2	/* Critical conditions */
#define LOGLEVEL_ERR		3	/* Error conditions */
#define LOGLEVEL_WARN		4	/* Warning conditions */
#define LOGLEVEL_NOTICE		5	/* Normal but significant condition */
#define LOGLEVEL_INFO		6	/* Informational */
#define LOGLEVEL_DEBUG		7	/* Debug-level messages */

#endif /* _SANDIX_KERN_LEVELS_H_ */
