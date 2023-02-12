/* 0xffff800000000000 initial of linear address */
/* 0xffff800000100000 initial of kernel address */
/* 0xffff800000a00000 initial of frame buffer */


.section            .text

.globl              _start

//============== enable temp lond mode

_start:
    mov             $0x10,      %ax
    mov             %ax,        %ds
    mov             %ax,        %es
    mov             %ax,        %fs
    mov             %ax,        %ss
    mov             $0x7e00,    %esp

//============== load GDTR

    lgdt            GDT_POINTER(%rip)                   /* displacement($rip) = [rip + displacement] */

//============== load IDTR

    lidt            IDT_POINTER(%rip)                   /* displacement will access memory RIP+-2GB */

    mov             $0x10,      %ax
    mov             %ax,        %ds
    mov             %ax,        %es
    mov             %ax,        %fs
    mov             %ax,        %gs
    mov             %ax,        %ss

    movq            $0x7e00,    %rsp

//============== load cr3

    movq            $0x101000,  %rax
    movq            %rax,       %cr3
    movq            switch_seg(%rip),   %rax            /* simulate the function of lcall */
    pushq           $0x08
    pushq           %rax
    lretq                                               /* recovery (pop, return) the address of target */

//============== 64-bit mode code

switch_seg:
    .quad           entry64

entry64:
    movq            $0x10,      %rax
    movq            %rax,       %ds
    movq            %rax,       %es
    movq            %rax,       %gs
    movq            %rax,       %ss
    movq            $0xffff800000007e00,    %rsp

    movq            go_to_kernel(%rip),     %rax
    pushq           $0x08
    pushq           %rax
    lretq

go_to_kernel:
    .quad           Start_Kernel

//============== init page

.align              8                       /* align code for every ? bytes or ! times of ? bytes */
                                            /* 2^8=128b=16B for i386 using a.out format, and the arm; 8B for i386 using elf format */
.org                0x1000                  /* start code at original address 0x1000 */

__PML4E:
    .quad           0x102007
    .fill           255,8,0
    .quad           0x102007
    .fill           255,8,0

.org                0x2000                  /* .org address of this program */
                                            /* 0xffff800000000000 + 0x100000 + 0x2000 */
__PDPTE:
    .quad           0x103003
    .fill           511,8,0

.org                0x3000

__PDE:
    .quad           0x000083                /* map 10MB memory to linear address 0 and 0xffff800000000000 */
    .quad           0x200083
    .quad           0x400083
    .quad           0x600083
    .quad           0x800083
    .quad           0xe0000083              /* map 16MB memory to linear address 0xa00000 and 0xffff800000a00000 */
    .quad           0xe0200083
    .quad           0xe0400083
    .quad           0xe0600083              /* 0x1000000 */
    .quad           0xe0800083
    .quad           0xe0a00083
    .quad           0xe0c00083
    .quad           0xe0e00083
    .fill           499,8,0                 /* fill last 499 page table entry with 0 */


.section            .data

//============== GDT_Table

.globl              GDT_Table

GDT_Table:
    .quad           0x0000000000000000      /* 0 NULL descriptor 00 */
    .quad           0x0020980000000000      /* 1 KERNEL Code 64-bit Segment 08 */
    .quad           0x0000920000000000      /* 2 KERNEL Data 64-bit Segment 10 */
    .quad           0x0020f80000000000      /* 3 USER Code 64-bit Segment 18 */
    .quad           0x0000f20000000000      /* 4 USER Data 64-bit Segment 20 */
    .quad           0x00cf9a000000ffff      /* 5 KERNEL Code 32-bit Segment 28 */
    .quad           0x00cf92000000ffff      /* 6 KERNEL Data 32-bit Segment 30 */
    .fill           10,8,0                  /* 8~9 TSS (jmp one segment <7>) in long-mode 128-bit 40*/
GDT_END:

GDT_POINTER:
GDT_LIMIT:          .word       GDT_END - GDT_Table - 1
GDT_BASE:           .quad       GDT_Table

//============== IDT_Table

.globl              IDT_Table

IDT_Table:
    .fill           512,8,0                 /* .fill    repeat, size, value */
IDT_END:

IDT_POINTER:
IDT_LIMIT:          .word       IDT_END - IDT_Table - 1
IDT_BASE:           .quad       IDT_Table

//============== TSS64_Table

.globl              TSS64_Table

TSS64_Table:
    .fill           13,8,0
TSS64_END:

TSS64_POINTER:
TSS64_LIMIT:        .word       TSS64_END - TSS64_Table - 1
TSS64_BASE:         .quad       TSS64_Table




