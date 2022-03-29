/*
** SCCS ID:	@(#)x86pic.h	2.2	2/16/21
**
** File:	x86pic.h
**
** Author:	Warren R. Carithers
**
** Contributor:	K. Reek
**
** Description:	Definitions of constants and macros for the
**		Intel 8259 Programmable Interrupt Controller
**
*/

#ifndef _X86PIC_H_
#define	_X86PIC_H_

/*
** This section is based on parts of Sun's "sys/pic.h" include file.
** Some modifications have been made for readability (!?!?).
**
** Original headers follow.
*/

/*
 * Copyright (c) 1992-1998 by Sun Microsystems, Inc.
 * All rights reserved.
 */

/*	Copyright (c) 1990, 1991 UNIX System Laboratories, Inc.	*/
/*	Copyright (c) 1984, 1986, 1987, 1988, 1989, 1990 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF		*/
/*	UNIX System Laboratories, Inc.				*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/* Definitions for 8259 Programmable Interrupt Controller */

#define	PIC_NEEDICW4	0x01		/* ICW4 needed */
#define	PIC_ICW1BASE	0x10		/* base for ICW1 */
#define	PIC_LTIM	0x08		/* level-triggered mode */
#define	PIC_86MODE	0x01		/* MCS 86 mode */
#define	PIC_AUTOEOI	0x02		/* do auto eoi's */
#define	PIC_SEC_BUF	0x08		/* put secondary in buffered mode */
#define	PIC_PRI_BUF	0x0C		/* put primary in buffered mode */
#define	PIC_SPFMODE	0x10		/* special fully nested mode */
#define	PIC_READISR	0x0B		/* Read the ISR */
#define	PIC_READIRR	0x0A		/* Read the IRR */
#define	PIC_EOI		0x20		/* Non-specific EOI command */
#define	PIC_SEOI	0x60		/* specific EOI command */
#define	PIC_SEOI_LVL7	(PIC_SEOI | 0x7)	/* specific EOI for level 7 */

/*
 * Interrupt configuration information specific to a particular computer.
 * These constants are used to initialize tables in modules/pic/space.c.
 * NOTE: The primary pic must always be pic zero.
 */

#define	NPIC		2		/* 2 PICs */
/*
** Port addresses for the command port and interrupt mask register port
** for both the primary and secondary PICs.
*/
#define	PIC_PRI_CMD_PORT	0x20	/* primary command */
#define	PIC_PRI_IMR_PORT	0x21	/* primary intr mask register */
#define	PIC_SEC_CMD_PORT	0xA0	/* secondary command */
#define	PIC_SEC_IMR_PORT	0xA1	/* secondary intr mask register */
#define	PIC_PRI_SEC_LINE	0x04	/* bit mask: secondary id */
#define PIC_SEC_ID		0x02	/* integer: secondary id */
#define	PIC_BUFFERED		0	/* PICs not in buffered mode */

#endif
