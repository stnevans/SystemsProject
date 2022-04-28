# 0 "boot/bootstrap.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "boot/bootstrap.S"
# 30 "boot/bootstrap.S"
 .code16

# 1 "include/bootstrap.h" 1
# 33 "boot/bootstrap.S" 2

BOOT_SEGMENT = 0x07C0
BOOT_ADDRESS = 0x00007C00
START_SEGMENT = 0x0000
START_OFFSET = 0x00007E00
SECTOR_SIZE = 0x200
BOOT_SIZE = (SECTOR_SIZE + SECTOR_SIZE)
OFFSET_LIMIT = 65536 - SECTOR_SIZE

MMAP_MAX_ENTRIES = (BOOT_ADDRESS - 0x00002D00 - 4) / 24




 .globl begtext

 .text
begtext:




 movw $BOOT_SEGMENT, %ax
 movw %ax, %ds
 movw %ax, %ss
 movw $0x4000, %ax
 movw %ax, %sp




 movb $0x01, %ah
 movb drive, %dl
 int $0x13
 jnc diskok

 movw $err_diskstatus, %si
 call dispMsg
 jmp .

diskok:
 movw $0,%ax
 movb drive,%dl
 int $0x13


 xorw %ax, %ax
 movw %ax, %es
 movw %ax, %di
 movb $0x08, %ah
 movb drive, %dl
 int $0x13


 andb $0x3F, %cl
 incb %cl
 incb %dh

 movb %cl, max_sec
 movb %dh, max_head






 movw $msg_loading,%si
 call dispMsg

 movw $1,%ax
 movw $START_SEGMENT,%bx
 movw %bx,%es
 movw $START_OFFSET,%bx
 call readprog






 movw $firstcount,%di

 pushw %ds
 movw (%di), %bx
 movw $0x000002D0, %ax
 movw %ax, %ds
 movw %bx, 0x0a
 popw %ds

nextblock:
 movw (%di),%ax
 testw %ax,%ax
 jz done_loading

 subw $2,%di
 movw (%di),%bx
 movw %bx,%es
 subw $2,%di
 movw (%di),%bx
 subw $2,%di
 pushw %di
 call readprog
 popw %di
 jmp nextblock







readprog:
 pushw %ax

 movw $3,%cx
retry:
 pushw %cx

 movw sec,%cx
 movw head,%dx
 movb drive, %dl

 movw $0x0201,%ax
 int $0x13
 jnc readcont

 movw $err_diskread,%si
 call dispMsg
 popw %cx
 loop retry
 movw $err_diskfail,%si
 call dispMsg
 jmp .

readcont:
 movw $msg_dot,%si
 call dispMsg
 cmpw $OFFSET_LIMIT,%bx
 je adjust
 addw $SECTOR_SIZE,%bx
 jmp readcont2

adjust:
 movw $0, %bx
 movw %es, %ax
 addw $0x1000,%ax
 movw %ax, %es

readcont2:
 incb %cl
 cmpb max_sec, %cl
 jnz save_sector

 movb $1, %cl
 incb %dh
 cmpb max_head, %dh
 jnz save_sector

 xorb %dh, %dh
 incb %ch
 cmpb $80, %ch
 jnz save_sector

 movw $err_toobig, %si
 call dispMsg
 jmp .

save_sector:
 movw %cx,sec
 movw %dx,head

 popw %ax
 popw %ax
 decw %ax
 jg readprog

readdone:
 movw $msg_bar,%si
 call dispMsg
 ret





done_loading:
 movw $msg_go, %si
 call dispMsg

 jmp switch




dispMsg:
 pushw %ax
 pushw %bx
repeat:
 lodsb

 movb $0x0e, %ah
 movw $0x07, %bx
 orb %al, %al
 jz getOut

 int $0x10
 jmp repeat

getOut:
 popw %bx
 popw %ax
 ret
# 292 "boot/bootstrap.S"
move_gdt:
 movw %cs, %si
 movw %si, %ds
 movw $start_gdt + BOOT_ADDRESS, %si
 movw $0x00000050, %di
 movw %di, %es
 xorw %di, %di
 movl $gdt_len, %ecx
 cld
 rep movsb
 ret






sec: .word 2
head: .word 0
max_sec: .byte 19
max_head: .byte 2




msg_loading:
 .asciz "Loading"
msg_dot:
 .asciz "."
msg_go:
 .asciz "done."
msg_bar:
 .asciz "|"




err_diskstatus:
 .asciz "Disk not ready.\n\r"
err_diskread:
 .asciz "Read failed\n\r"
err_toobig:
 .asciz "Too big\n\r"
err_diskfail:
 .asciz "Can't proceed\n\r"
# 345 "boot/bootstrap.S"
gdt_48:
 .word 0x2000
 .quad 0x00000500

idt_48:
 .word 0x0800
 .quad 0x00002500
# 370 "boot/bootstrap.S"
 .org SECTOR_SIZE-4

drive: .word 0x80

boot_sig:
 .word 0xAA55
# 420 "boot/bootstrap.S"
check_memory:


 pushw %ds
 pushw %es
 pushw %ax
 pushw %bx
 pushw %cx
 pushw %dx
 pushw %si
 pushw %di


 movw $0x000002D0, %bx
 mov %bx, %ds
 mov %bx, %es


 movw $0x4, %di

 movw $1, %es:20(%di)

 xorw %bp, %bp
 xorl %ebx, %ebx

 movl $0x534D4150, %edx
 movl $0xE820, %eax
 movl $24, %ecx
 int $0x15


 jc cm_failed
 movl $0x534D4150, %edx
 cmpl %eax, %edx
 jne cm_failed
 testl %ebx, %ebx
 je cm_failed

 jmp cm_jumpin

cm_loop:
 movl $0xE820, %eax
 movw $1, 20(%di)
 movl $24, %ecx
 int $0x15
 jc cm_end_of_list
 movl $0x534D4150, %edx

cm_jumpin:
 jcxz cm_skip_entry

 cmp $20, %cl
 jbe cm_no_text

 testb $1, %es:20(%di)
 je cm_skip_entry

cm_no_text:
 mov %es:8(%di), %ecx
 or %es:12(%di), %ecx
 jz cm_skip_entry

 inc %bp


 cmpw $MMAP_MAX_ENTRIES, %bp
 jge cm_end_of_list


 add $24, %di

cm_skip_entry:

 testl %ebx, %ebx
 jne cm_loop

cm_end_of_list:

 movw %bp, %ds:0x0

 clc
 jmp cm_ret

cm_failed:
 movl $-1, %ds:0x0
 stc

cm_ret:


 popw %di
 popw %si
 popw %dx
 popw %cx
 popw %bx
 popw %ax
 popw %es
 popw %ds
 ret
# 528 "boot/bootstrap.S"
switch:
 cli
 movb $0x80, %al
 outb %al, $0x70

 call floppy_off
 call enable_A20
 call move_gdt

 call check_memory







 lidt idt_48 + BOOT_ADDRESS
 lgdt gdt_48 + BOOT_ADDRESS

 movl %cr0, %eax
 orl $1, %eax
 movl %eax, %cr0
# 569 "boot/bootstrap.S"
 .byte 0x66
 .code32
 ljmp $0x0010, $0x00010000
 .code16






floppy_off:
 push %dx
 movw $0x3f2, %dx
 xorb %al, %al
 outb %al, %dx
 pop %dx
 ret




enable_A20:
 call a20wait
 movb $0xad, %al
 outb %al, $0x64

 call a20wait
 movb $0xd0, %al
 outb %al, $0x64

 call a20wait2
 inb $0x60, %al
 pushl %eax

 call a20wait
 movb $0xd1, %al
 outb %al, $0x64

 call a20wait
 popl %eax
 orb $2, %al
 outb %al, $0x60

 call a20wait
 mov $0xae, %al
 out %al, $0x64

 call a20wait
 ret

a20wait:
 movl $65536, %ecx
wait_loop:
 inb $0x64, %al
 test $2, %al
 jz wait_exit
 loop wait_loop
 jmp a20wait
wait_exit:
 ret

a20wait2:
 mov $65536, %ecx
wait2_loop:
 in $0x64, %al
 test $1, %al
 jnz wait2_exit
 loop wait2_loop
 jmp a20wait2
wait2_exit:
 ret





start_gdt:
 .word 0,0,0,0

linear_seg:
 .word 0xFFFF
 .word 0x0000
 .byte 0x00
 .byte 0x92
 .byte 0xCF
 .byte 0x00

code_seg:
 .word 0xFFFF
 .word 0x0000
 .byte 0x00
 .byte 0x9A
 .byte 0xCF
 .byte 0x00

data_seg:
 .word 0xFFFF
 .word 0x0000
 .byte 0x00
 .byte 0x92
 .byte 0xCF
 .byte 0x00

stack_seg:
 .word 0xFFFF
 .word 0x0000
 .byte 0x00
 .byte 0x92
 .byte 0xCF
 .byte 0x00

end_gdt:
gdt_len = end_gdt - start_gdt
# 699 "boot/bootstrap.S"
 .org 1024-2
firstcount:
 .word 0
