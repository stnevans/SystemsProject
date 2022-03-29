/*
** SCCS ID:	@(#)cio.h	2.2	3/10/20
**
** File:	cio.h
**
** Author:	K. Reek
**
** Contributor:
**
** Based on:	c_io.c 1.13 (Ken Reek, Jon Coles, Warren R. Carithers)
**
** Description:	Declarations and descriptions of console I/O routines
**
**	These routines provide a rudimentary capability for printing to
**	the screen and reading from the keyboard.  
**
** Screen output:
**	There are two families of functions.  The first provides a window
**	that behaves in the usual manner: writes extending beyond the right
**	edge of the window wrap around to the next line, the top line
**	scrolls off the window to make room for new lines at the bottom.
**	However, you may choose what part of the screen contains this
**	scrolling window.  This allows you to print some text at fixed
**	locations on the screen while the rest of the screen scrolls.
**
**	The second family allows for printing at fixed locations on the
**	screen.  No scrolling or line wrapping are done for these functions.
**	It is not intended that these functions be used to write in the
**	scrolling area of the screen.
**
**	In both sets of functions, the (x,y) coordinates are interpreted
**	as (column,row), with the upper left corner of the screen being
**	(0,0) and the lower right corner being (79,24).
**
**	The printf provided in both sets of functions has the same
**	conversion capabilities.  Format codes are of the form:
**
**		%-0WC
**
**	where "-", "0", and "W" are all optional:
**	  "-" is the left-adjust flag (default is right-adjust)
**	  "0" is the zero-fill flag (default is space-fill)
**	  "W" is a number specifying the minimum field width (default: 1 )
**	and "C" is the conversion type, which must be one of these:
**	  "c" print a single character
**	  "s" print a null-terminated string
**	  "d" print an integer as a decimal value
**	  "x" print an integer as a hexadecimal value
**	  "o" print an integer as a octal value
**
** Keyboard input:
**	Two functions are provided: getting a single character and getting
**	a newline-terminated line.  A third function returns a count of
**	the number of characters available for immediate reading. 
**	No conversions are provided (yet).
*/

#ifndef _CIO_H_
#define _CIO_H_

// EOT indicator (control-D)
#define EOT '\04'

/*
** Name:	__cio_init
**
** Description:	Initializes the I/O routines.  This is called by the
**		standalone loader so you need not call it.
** Argument:	pointer to an input notification function, or NULL
*/
void __cio_init( void (*notify)(int) );

/*****************************************************************************
**
** SCROLLING OUTPUT ROUTINES
**
**	Each operation begins at the current cursor position and advances
**	it.  If a newline is output, the reminder of that line is cleared.
**	Output extending past the end of the line is wrapped.  If the
**	cursor is moved below the scrolling region's bottom edge, scrolling
**	is delayed until the next output is produced.
*/

/*
** Name:	__cio_setscroll
**
** Description:	This sets the scrolling region to be the area defined by
**		the arguments.  The remainder of the screen does not scroll
**		and may be used to display data you do not want to move.
**		By default, the scrolling region is the entire screen .
** Arguments:	coordinates of upper-left and lower-right corners of region
*/
void __cio_setscroll( unsigned int min_x, unsigned int min_y,
		  unsigned int max_x, unsigned int max_y );

/*
** Name:	__cio_moveto
**
** Description:	Moves the cursor to the specified position. (0,0) indicates
**		the upper left corner of the scrolling region.  Subsequent
**		output will begin at the cursor position.
** Arguments:	desired cursor position
*/
void __cio_moveto( unsigned int x, unsigned int y );

/*
** Name:	__cio_putchar
**
** Description:	Prints a single character.
** Arguments:	the character to be printed
*/
void __cio_putchar( unsigned int c );

/*
** Name:	__cio_puts
**
** Description:	Prints the characters in the string up to but not including
**		the terminating null byte.
** Arguments:	pointer to a null-terminated string
*/
void __cio_puts( char *str );

/*
** Name:    __cio_write
**
** Description: Prints "length" characters from the buffer.
** Arguments:   Pointer to a character buffer, and the size of the buffer
*/
void __cio_write( const char *buf, int length );

/*
** Name:	__cio_printf
**
** Description:	Limited form of printf (see the beginning of this file for
**		a list of what is implemented).
** Arguments:	printf-style format and optional values
*/
void __cio_printf( char *fmt, ... );

/*
** Name:	__cio_scroll
**
** Description:	Scroll the scrolling region up by the given number of lines.
**		The output routines scroll automatically so normally you
**		do not need to call this routine yourself.
** Arguments:	number of lines
*/
void __cio_scroll( unsigned int lines );

/*
** Name:	__cio_clearscroll
**
** Description:	Clears the entire scrolling region to blank spaces, and
**		moves the cursor to (0,0).
*/
void __cio_clearscroll( void );

/*****************************************************************************
**
** NON-SCROLLING OUTPUT ROUTINES
**
**	Coordinates are relative to the entire screen: (0,0) is the upper
**	left corner.  There is no line wrap or scrolling.
*/

/*
** Name:	__cio_putchar_at
**
** Description:	Prints the given character.  If a newline is printed,
**		the rest of the line is cleared.  If this happens to the
**		left of the scrolling region, the clearing stops when the
**		region is reached.  If this happens inside the scrolling
**		region, the clearing stops when the edge of the region
**		is reached.
** Arguments:	coordinates, the character to be printed
*/
void __cio_putchar_at( unsigned int x, unsigned int y, unsigned int c );

/*
** Name:	__cio_puts_at
**
** Description:	Prints the given string.  __cio_putchar_at is used to print
**		the individual characters; see that description for details.
** Arguments:	coordinates, null-terminated string to be printed
*/
void __cio_puts_at( unsigned int x, unsigned int y, char *str );

/*
** Name:	__cio_printf_at
**
** Description:	Limited form of printf (see the beginning of this file for
**		a list of what is implemented).
** Arguments:	coordinates, printf-style format, optional values
*/
void __cio_printf_at( unsigned int x, unsigned int y, char *fmt, ... );

/*
** Name:	__cio_clearscreen
**
** Description:	This function clears the entire screen, including the
**		scrolling region.
*/
void __cio_clearscreen( void );

/*****************************************************************************
**
** INPUT ROUTINES
**
**	When interrupts are enabled, a keyboard ISR collects keystrokes
**	and saves them until the program calls for them.  If the input
**	queue fills, additional characters are silently discarded.
**	When interrupts are not enabled, keystrokes made when no input
**	routines have been **	called are lost.  This can cause errors in
**	the input translation because the states of the Shift and Ctrl keys
**	may not be tracked accurately.  If interrupts are disabled, the user
**	is advised to refrain from typing anything except when the program is
**	waiting for input.
*/

/*
** Name:	__cio_getchar
**
** Description:	If the character is not immediately available, the function
**		waits until the character arrives.
** Returns:	The next character typed on the keyboard.
*/
int __cio_getchar( void );

/*
** Name:	__cio_gets
**
** Description:	This function reads a newline-terminated line from the
**		keyboard.  __cio_getchar is used to obtain the characters;
**		see that description for more details.  The function
**		returns when:
**			a newline is entered (this is stored in the buffer)
**			ctrl-D is entered (not stored in the buffer)
**			the buffer becomes full.
**		The buffer is null-terminated in all cases.
** Arguments:	pointer to input buffer, size of buffer
** Returns:	count of the number of characters read
*/
int __cio_gets( char *buffer, unsigned int size );

/*
** Name:	__cio_input_queue
**
** Description:	This function lets the program determine whether there
**		is input available.  This determines whether or not a call
**		to __cio_getchar would block.
** Returns:	number of characters in the input queue
*/
int __cio_input_queue( void );

#endif
