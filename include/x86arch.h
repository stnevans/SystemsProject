/*
** SCCS ID:	@(#)x86arch.h	2.1	12/8/19
**
** File:	x86arch.h
**
** Author:	Warren R. Carithers
**
** Contributor:	K. Reek
**
** Description:	Definitions of constants and macros for use
**		with the x86 architecture and registers.
**
*/

#ifndef _X86ARCH_H_
#define	_X86ARCH_H_

/*
** Video stuff
*/
#define	VIDEO_BASE_ADDR		0xB8000

/*
** Memory management
*/
#define	SEG_PRESENT	0x80
#define	SEG_PL_0	0x00
#define	SEG_PL_1	0x20
#define	SEG_PL_2	0x40
#define	SEG_PL_3	0x50
#define	SEG_SYSTEM	0x00
#define	SEG_NON_SYSTEM	0x10
#define	SEG_32BIT	0x04
#define	DESC_IGATE	0x06

/*
** Exceptions
*/
#define	N_EXCEPTIONS              256

/*
** Bit definitions in registers
**
** See IA-32 Intel Architecture SW Dev. Manual, Volume 3: System
** Programming Guide, page 2-8.
*/

/*
** EFLAGS
*/
#define	EFLAGS_RSVD	0xffc00000	/* reserved */
#define	EFLAGS_MB0	0x00008020	/* must be zero */
#define	EFLAGS_MB1	0x00000002	/* must be 1 */

#define	EFLAGS_ID	0x00200000
#define	EFLAGS_VIP	0x00100000
#define	EFLAGS_VIF	0x00080000
#define	EFLAGS_AC	0x00040000
#define	EFLAGS_VM	0x00020000
#define	EFLAGS_RF	0x00010000
#define	EFLAGS_NT	0x00004000
#define	EFLAGS_IOPL	0x00003000
#define	EFLAGS_OF	0x00000800
#define	EFLAGS_DF	0x00000400
#define	EFLAGS_IF	0x00000200
#define	EFLAGS_TF	0x00000100
#define	EFLAGS_SF	0x00000080
#define	EFLAGS_ZF	0x00000040
#define	EFLAGS_AF	0x00000010
#define	EFLAGS_PF	0x00000004
#define	EFLAGS_CF	0x00000001

/*
** CR0, CR1, CR2, CR3, CR4
**
** IA-32 V3, page 2-12.
*/
#define	CR0_RSVD	0x1ffaffc0
#define CR0_PG		0x80000000
#define CR0_CD		0x40000000
#define CR0_NW		0x20000000
#define CR0_AM		0x00040000
#define CR0_WP		0x00010000
#define CR0_NE		0x00000020
#define CR0_ET		0x00000010
#define CR0_TS		0x00000008
#define CR0_EM		0x00000004
#define CR0_MP		0x00000002
#define CR0_PE		0x00000001
#define	CR1_RSVD	0xffffffff
#define	CR2_RSVD	0x00000000
#define	CR2_PF_LIN_ADDR	0xffffffff
#define	CR3_RSVD	0x00000fe7
#define CR3_PD_BASE	0xfffff000
#define CR3_PCD		0x00000010
#define CR3_PWT		0x00000008
#define	CR4_RSVD	0xfffff800
#define CR4_OSXMMEXCPT	0x00000400
#define CR4_OSFXSR	0x00000200
#define CR4_PCE		0x00000100
#define CR4_PGE		0x00000080
#define CR4_MCE		0x00000040
#define CR4_PAE		0x00000020
#define CR4_PSE		0x00000010
#define CR4_DE		0x00000008
#define CR4_TSD		0x00000004
#define CR4_PVI		0x00000002
#define CR4_VME		0x00000001

/*
** PMode segment selectors
**
** IA-32 V3, page 3-8.
*/
#define	SEG_SEL_IX	0xfff8
#define	SEG_SEL_TI	0x0004
#define	SEG_SEL_RPL	0x0003

/*
** Segment descriptor bytes
**
** IA-32 V3, page 3-10.
*/
	/* byte 5 - access control */
#define	SEG_ACCESS_TYPE_MASK	0x0f
#	define		SEG_DATA_A_BIT		0x1
#	define		SEG_DATA_W_BIT		0x2
#	define		SEG_DATA_E_BIT		0x4
#	define		SEG_CODE_A_BIT		0x1
#	define		SEG_CODE_R_BIT		0x2
#	define		SEG_CODE_C_BIT		0x4
#		define	SEG_DATA_RO		0x0
#		define	SEG_DATA_ROA		0x1
#		define	SEG_DATA_RW		0x2
#		define	SEG_DATA_RWA		0x3
#		define	SEG_DATA_RO_XD		0x4
#		define	SEG_DATA_RO_XDA		0x5
#		define	SEG_DATA_RW_XW		0x6
#		define	SEG_DATA_RW_XWA		0x7
#		define	SEG_CODE_XO		0x8
#		define	SEG_CODE_XOA		0x9
#		define	SEG_CODE_XR		0xa
#		define	SEG_CODE_XRA		0xb
#		define	SEG_CODE_XO_C		0xc
#		define	SEG_CODE_XO_CA		0xd
#		define	SEG_CODE_XR_C		0xe
#		define	SEG_CODE_XR_CA		0xf

#define	SEG_ACCESS_S_BIT	0x10
#	define		SEG_S_SYSTEM		0x00
#	define		SEG_S_NON_SYSTEM	0x10

#define	SEG_ACCESS_DPL_MASK	0x60
#	define		SEG_DPL_0		0x00
#	define		SEG_DPL_1		0x20
#	define		SEG_DPL_2		0x40
#	define		SEG_DPL_3		0x60

#define	SEG_ACCESS_P_BIT	0x80

	/* byte 6 - sizes */
#define	SEG_SIZE_LIM_19_16	0x0f

#define	SEG_SIZE_AVL_BIT	0x10

#define	SEG_SIZE_D_B_BIT	0x40
#	define		SEG_DB_16BIT		0x00
#	define		SEG_DB_32BIT		0x40

#define	SEG_SIZE_G_BIT		0x80
#	define		SEG_GRAN_BYTE		0x00
#	define		SEG_GRAN_4KBYTE		0x80

/*
** System-segment and gate-descriptor types
**
** IA-32 V3, page 3-15.
*/
	/* type 0 reserved */
#define	SEG_SYS_16BIT_TSS_AVAIL		0x1
#define	SEG_SYS_LDT			0x2
#define	SEG_SYS_16BIT_TSS_BUSY		0x3
#define	SEG_SYS_16BIT_CALL_GATE		0x4
#define	SEG_SYS_TASK_GATE		0x5
#define	SEG_SYS_16BIT_INT_GATE		0x6
#define	SEG_SYS_16BIT_TRAP_GATE		0x7
	/* type 8 reserved */
#define	SEG_SYS_32BIT_TSS_AVAIL		0x9
	/* type A reserved */
#define	SEG_SYS_32BIT_TSS_BUSY		0xb
#define	SEG_SYS_32BIT_CALL_GATE		0xc
	/* type D reserved */
#define	SEG_SYS_32BIT_INT_GATE		0xe
#define	SEG_SYS_32BIT_TRAP_GATE		0xf

/*
** IDT Descriptors
** 
** IA-32 V3, page 5-13.
**
** All have a segment selector in bytes 2 and 3; Task Gate descriptors
** have bytes 0, 1, 4, 6, and 7 reserved; others have bytes 0, 1, 6,
** and 7 devoted to the 16 bits of the Offset, with the low nybble of
** byte 4 reserved.
*/
#define	IDT_PRESENT		0x8000
#define	IDT_DPL_MASK		0x6000
#	define		IDT_DPL_0	0x0000
#	define		IDT_DPL_1	0x2000
#	define		IDT_DPL_2	0x4000
#	define		IDT_DPL_3	0x6000
#define	IDT_GATE_TYPE		0x0f00
#	define		IDT_TASK_GATE	0x0500
#	define		IDT_INT16_GATE	0x0600
#	define		IDT_INT32_GATE	0x0e00
#	define		IDT_TRAP16_GATE	0x0700
#	define		IDT_TRAP32_GATE	0x0f00

/*
** Interrupt vectors
*/
#define	INT_VEC_DIVIDE_ERROR		0x00
#define	INT_VEC_DEBUG_EXCEPTION		0x01
#define	INT_VEC_NMI_INTERRUPT		0x02
#define	INT_VEC_BREAKPOINT		0x03
#define	INT_VEC_INTO_DETECTED_OVERFLOW	0x04
#define	INT_VEC_BOUND_RANGE_EXCEEDED	0x05
#define	INT_VEC_INVALID_OPCODE		0x06
#define	INT_VEC_DEVICE_NOT_AVAILABLE	0x07
#define	INT_VEC_DOUBLE_EXCEPTION	0x08
#define	INT_VEC_COPROCESSOR_OVERRUN	0x09
#define	INT_VEC_INVALID_TSS		0x0a
#define	INT_VEC_SEGMENT_NOT_PRESENT	0x0b
#define	INT_VEC_STACK_FAULT		0x0c
#define	INT_VEC_GENERAL_PROTECTION	0x0d
#define	INT_VEC_PAGE_FAULT		0x0e

#define	INT_VEC_COPROCESSOR_ERROR	0x10
#define	INT_VEC_ALIGNMENT_CHECK		0x11
#define	INT_VEC_MACHINE_CHECK		0x12
#define	INT_VEC_SIMD_FP_EXCEPTION	0x13

#define	INT_VEC_TIMER			0x20
#define	INT_VEC_KEYBOARD		0x21

#define	INT_VEC_SERIAL_PORT_2		0x23
#define	INT_VEC_SERIAL_PORT_1		0x24
#define	INT_VEC_PARALLEL_PORT		0x25
#define	INT_VEC_FLOPPY_DISK		0x26
#define	INT_VEC_MYSTERY			0x27
#define	INT_VEC_MOUSE			0x2c

#endif
