/*
** SCCS ID:	@(#)x86pit.h	2.1	12/8/19
**
** File:	x86pic.h
**
** Author:	Warren R. Carithers
**
** Contributor:	K. Reek
**
** Description:	Definitions of constants and macros for the
**		Intel 8254 Programmable Interval Timer
**
*/

#ifndef _X86PIT_H_
#define	_X86PIT_H_


/*
** Hardware timer (Intel 8254 Programmable Interval Timer)
*/

#define	TIMER_DEFAULT_TICKS_PER_SECOND	18	/* default ticks per second */
#define	TIMER_DEFAULT_MS_PER_TICK	(1000/TIMER_DEFAULT_TICKS_PER_SECOND)
#define	TIMER_FREQUENCY			1193182	/* clock cycles/sec  */
#define	TIMER_BASE_PORT			0x40	/* I/O port for the timer */
#define	TIMER_0_PORT			( TIMER_BASE_PORT )
#define	TIMER_1_PORT			( TIMER_BASE_PORT + 1 )
#define	TIMER_2_PORT			( TIMER_BASE_PORT + 2 )
#define	TIMER_CONTROL_PORT		( TIMER_BASE_PORT + 3 )
#define	TIMER_USE_DECIMAL		0x01	/* binary counter (default) */
#define	TIMER_USE_BCD			0x01	/* BCD counter */

/* Timer modes */
#define	TIMER_MODE_0			0x00	/* int on terminal count */
#define	TIMER_MODE_1			0x02	/* one-shot */
#define	TIMER_MODE_2			0x04	/* divide-by-N */
#define	TIMER_MODE_3			0x06	/* square-wave */
#define	TIMER_MODE_4			0x08	/* software strobe */
#define	TIMER_MODE_5			0x0a	/* hardware strobe */

/* Timer 0 settings */
#define	TIMER_0_SELECT			0x00	/* select timer 0 */
#define	TIMER_0_LOAD			0x30	/* load LSB, then MSB */
#define	TIMER_0_NDIV		TIMER_MODE_2	/* divide-by-N counter */
#define	TIMER_0_SQUARE		TIMER_MODE_3	/* square-wave mode */
#define	TIMER_0_ENDSIGNAL		0x00	/* assert OUT at end of count */

/* Timer 1 settings */

#define	TIMER_1_SELECT			0x40	/* select timer 1 */
#define	TIMER_1_READ			0x30	/* read/load LSB then MSB */
#define	TIMER_1_RATE			0x06	/* square-wave, for USART */

/* Timer 2 settings */
#define	TIMER_2_SELECT			0x80	/* select timer 1 */
#define	TIMER_2_READ			0x30	/* read/load LSB then MSB */
#define	TIMER_2_RATE			0x06	/* square-wave, for USART */

/* Timer read-back */
#define	TIMER_READBACK			0xc0	/* perform a read-back */
#define	TIMER_RB_NOT_COUNT		0x20	/* don't latch the count */
#define	TIMER_RB_NOT_STATUS		0x10	/* don't latch the status */
#define	TIMER_RB_CHAN_2			0x08	/* read back channel 2 */
#define	TIMER_RB_CHAN_1			0x04	/* read back channel 1 */
#define	TIMER_RB_CHAN_0			0x02	/* read back channel 0 */
#define	TIMER_RB_ACCESS_MASK		0x30	/* access mode field */
#define	TIMER_RB_OP_MASK		0x0e	/* oper mode field */
#define	TIMER_RB_BCD_MASK		0x01	/* BCD mode field */

#endif
