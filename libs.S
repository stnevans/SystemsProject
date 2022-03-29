/**
** @file libs.S
**
** @author Jon Coles
** @author Warren R. Carithers
** @author K. Reek
**
** Support library functions (assembly language)
**
** These functions are support routines used in various places
** throughout the framework.  They are written in assembly language
** for efficiency and/or because they require access to machine-level
** features that are hard to access from C.
*/

#define SP_KERNEL_SRC
#define SP_ASM_SRC

/*
** Parameter offsets
*/
ARG1	= 8			// Offset to 1st argument
ARG2	= 12			// Offset to 2nd argument

/**
** Name:	__inb, __inw, __inl
**
** Description: read a single byte, word, or longword from the specified
**		input port
**
** usage:  data = __in*( unsigned short port );
**
** @param port   The port from which to read
**
** @return The data from that port
*/
	.globl	__inb, __inw, __inl

__inb:
	pushl	%ebp
	movl	%esp, %ebp
	xorl	%eax, %eax  	  // Clear the high order bytes of %eax
	movl	ARG1(%ebp), %edx  // Move port number to %edx
	inb 	(%dx)	    	  // Get a byte from the port into %al (low
	popl	%ebp	    	  //   byte of %eax)
	ret

__inw:
	pushl	%ebp
	movl	%esp, %ebp
	xorl	%eax, %eax        // Clear the high order bytes of %eax
	movl	ARG1(%ebp), %edx  // Move port number to %edx
	inw 	(%dx)		  // Get a word from the port into %ax (low
	popl	%ebp		  //   word of %eax)
	ret

__inl:
	pushl	%ebp
	movl	%esp, %ebp
	xorl	%eax, %eax	  // Clear the high order bytes of %eax
	movl	ARG1(%ebp), %edx  // Move port number to %edx
	inl 	(%dx)		  // Get a longword from the port into %eax
	popl	%ebp
	ret
	
/**
** Name:	__outb, __outw, __outl
**
** Description: write a single byte, word, or longword to the specified
**		output port
**
** usage:  __out*( uint16_t port, uint* data );
**
** @param port   The port to be written to
** @param data   The data to write to that port
*/
	.globl	__outb, __outw, __outl
__outb:
	pushl	%ebp
	movl	%esp, %ebp
	movl	ARG1(%ebp), %edx	// Get the port number into %edx,
	movl	ARG2(%ebp), %eax	//   and the value into %eax
	outb	(%dx)		        // Output that byte to the port
	popl	%ebp		        //   (only %al is sent)
	ret

__outw:
	pushl	%ebp
	movl	%esp, %ebp
	movl	ARG1(%ebp), %edx	// Get the port number into %edx,
	movl	ARG2(%ebp), %eax	//   and the value into %eax
	outw	(%dx)		        // Output that word to the port.
	popl	%ebp		        //   (only %ax is sent)
	ret

__outl:
	pushl	%ebp
	movl	%esp, %ebp
	movl	ARG1(%ebp), %edx	// Get the port number into %edx,
	movl	ARG2(%ebp), %eax	//   and the value into %eax
	outl	(%dx)		        // Output that longword to the port.
	popl	%ebp
	ret

/**
** __get_flags: return the current processor flags
**
** usage:  unsigned int __get_flags( void );
**
** @return The EFLAGS register after entry to this function
*/
	.globl	__get_flags

__get_flags:
	pushfl			// Push flags on the stack,
	popl	%eax	//   and pop them into eax.
	ret

/**
** __pause: halt until something happens
**      void __pause( void );
*/
	.globl	__pause

__pause:
	pushl	%ebp
	movl	%esp, %ebp
	sti
	hlt
	popl	%ebp
	ret

/**
** __get_ra: get the return address for the calling function
**           (i.e., where whoever called us will go back to)
**	uint32_t __get_ra( void );
**
** @return The address the calling routine will return to as a uint32_t
*/
	.global	__get_ra

__get_ra:
	// DO NOT create a stack frame - use the caller's
	//
	// Caller's return address is between the saved EBP
	// and its first parameter
	movl	4(%ebp), %eax
	ret
