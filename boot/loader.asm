    org             10000h

    jmp	            Label_Start

    %include        "fat12.inc"                         ; include FAT12 structure

BaseOfKernelFile    equ     0x00
OffsetOfKernelFile  equ     0x100000                    ; kernel starts at 1MB, it will be a flat memory space
                                                        ; BIOS only accesses memory up to 1MB
BaseTmpOfKernelAddr         equ     0x00
OffsetTmpOfKernelFile       equ     0x7e00              ; BIOS loads kernel at this place temporarily
MemoryStructBufferAddr      equ     0x7e00

[SECTION gdt]

LABEL_GDT:          dd      0,0
LABEL_DESC_CODE32:  dd      0x0000ffff,0x00cf9a00
LABEL_DESC_DATA32:  dd      0x0000ffff,0x00cf9200

GdtLen              equ     $ - LABEL_GDT
GdtPtr              dw      GdtLen - 1
                    dd      LABEL_GDT

SelectorCode32      equ     LABEL_DESC_CODE32 - LABEL_GDT
SelectorData32      equ     LABEL_DESC_DATA32 - LABEL_GDT

[SECTION gdt64]

LABEL_GDT64:        dq      0x0000000000000000
LABEL_DESC_CODE64:  dq      0x0020980000000000
LABEL_DESC_DATA64:  dq      0x0000920000000000

GdtLen64            equ     $ - LABEL_GDT64
GdtPtr64            dw      GdtLen64 - 1
                    dd      LABEL_GDT64

SelectorCode64      equ     LABEL_DESC_CODE64 - LABEL_GDT64
SelectorData64      equ     LABEL_DESC_DATA64 - LABEL_GDT64


[SECTION .s16]                                          ; define a section
[BITS 16]                                               ; inform the NASM that program runs with bit16 processor

Label_Start:
    mov             ax,     cs
    mov             ds,     ax
    mov             es,     ax
    mov             ax,     0x00
    mov             ss,     ax
    mov             sp,     0x7c00

;============== display on screen: Start Loader

    mov             ax,     1301h
    mov             bx,     000fh
    mov             dx,     0100h
    mov             cx,     12
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     StartLoaderMessage
    int             10h

;============== open address A20 (Fast Gate A20)        ; old processor only has 20 address lines and can access 1MB memory

    push            ax
    in              al,     92h                         ; read I/O port 0x92
    or              al,     00000010b                   ; bit1=1
    out             92h,    al                          ; write I/O port 0x92
    pop             ax

;============== open address A20 (int 15h)

    ;mov             ax,     2401h                       ; ax=2401h, open; ax=2400h, close; ax=2403h, check
    ;int             15h

    cli                                                 ; then close outer interruption

    db              0x66                                ; use bit32 instruction with bit16 processor; set 0x66 before instruction
    lgdt            [GdtPtr]                            ; load structure data of protect mode

    mov             eax,    cr0
    or              eax,    1
    mov             cr0,    eax                         ; set bit0 of cr0 to be 1; open protect mode

    mov             ax,     SelectorData32
    mov             fs,     ax                          ; into the big real mode; the real mode that accesses more than 1MB memory
    mov             eax,    cr0
    and             al,     11111110b
    mov             cr0,    eax                         ; set bit0 of cr0 to be 0; close protect mode

    sti                                                 ; open outer interruption

;==============	reset floppy

	xor	ah,	ah
	xor	dl,	dl
	int	13h

;============== search kernel.bin

    mov             word    [SectorNo],    SectorNumOfRootDirStart

Label_Search_In_Root_Dir_Begin:
    cmp             word    [RootDirSizeForLoop],   0
    jz              Label_No_KernelBin
    dec             word    [RootDirSizeForLoop]
    mov             ax,     00h
    mov             es,     ax
    mov             bx,     8000h                       ; es:bx, pointer of read buffer
    mov             ax,     [SectorNo]                  ; ax=19; SectorNo, pointer of the tmp variable
    mov             cl,     1
    call            Func_ReadOneSector
    mov             si,     KernelFileName
    mov             di,     8000h
    cld
    mov             dx,     10h

Label_Search_For_KernelBin:
    cmp             dx,     0
    jz              Label_Goto_Next_Sector_In_Root_Dir
    dec             dx
    mov             cx,     11                          ; the length of file name string; must be 11 for FAT12

Label_Cmp_FileName:
    cmp             cx,     0
    jz              Label_FileName_Found
    dec             cx
    lodsb
    cmp             al,     byte    [es:di]
    jz              Label_Go_On
    jmp             Label_Different

Label_Go_On:
    inc             di
    jmp             Label_Cmp_FileName

Label_Different:
    and             di,     0ffe0h
    add             di,     20h
    mov             si,     KernelFileName
    jmp             Label_Search_For_KernelBin

Label_Goto_Next_Sector_In_Root_Dir:
    add             word    [SectorNo],     1
    jmp             Label_Search_In_Root_Dir_Begin

;============== display on screen: ERROR: No Kernel Found

Label_No_KernelBin:
    mov             ax,     1301h
    mov             bx,     008ch 
    mov             dx,     0300h
    mov             cx,     22
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     NoKernelMessage
    int             10h

    jmp             $

;============== found kernel.bin name in root director struct

Label_FileName_Found:
    mov             ax,     RootDirSectors
    and             di,     0ffe0h
    add             di,     01ah
    mov             cx,     word    [es:di]
    push            cx
    add             cx,     ax
    add             cx,     SectorBalance               ; consruct the LBA_SectorNum
    mov             eax,    BaseTmpOfKernelAddr
    mov             es,     eax
    mov             bx,     OffsetTmpOfKernelFile       ; set read buffer to BaseOfLoader:OffsetOfLoader
    mov             ax,     cx

Label_Go_On_Loading_File:
    push            ax
    push            bx
    mov             ah,     0eh                         ; ah=0eh, display a char
    mov             al,     '.'                         ; read one sector, display one '.'
    mov             bl,     0fh                         ; bl, the properties and colors of char
    int             10h
    pop             bx
    pop             ax                                  ; use the LBA_SectorNum

    mov             cl,     1
    call            Func_ReadOneSector
    pop             ax

;;;;;;;;;;;;;;; move kernel
    push            cx
    push            eax
    push            fs
    push            edi
    push            ds
    push            esi

    mov             cx,     200h                        ; loop count = 512, one sector
    mov             ax,     BaseOfKernelFile
    mov             fs,     ax
    mov             edi,    dword   [OffsetOfKernelFileCount]

    mov             ax,     BaseTmpOfKernelAddr
    mov             ds,     ax
    mov             esi,    OffsetTmpOfKernelFile

Label_Mov_Kernel:
    mov             al,     byte    [ds:esi]
    mov             byte    [fs:edi],   al              ; move bytes one by one

    inc             esi                                 ; temp offset
    inc             edi                                 ; kernel offset

    loop            Label_Mov_Kernel

    mov             eax,    0x1000
    mov             ds,     eax

    mov             dword   [OffsetOfKernelFileCount],  edi

    pop             esi
    pop             ds
    pop             edi
    pop             fs
    pop             eax
    pop             cx
;;;;;;;;;;;;;;;

    call            Func_GetFATEntry
    cmp             ax,     0fffh                       ; FATEntry=0fffh, the end of the file
    jz              Label_File_Loaded
    push            ax
    mov             dx,     RootDirSectors
    add             ax,     dx
    add             ax,     SectorBalance

    jmp             Label_Go_On_Loading_File

Label_File_Loaded:
	;mov	            ax,     0B800h
	;mov	            gs,     ax
	;mov	            ah,     0Fh				            ; 0000: black background    1111: white foreground
	;mov	            al,     'G'
	;mov	            [gs:((80 * 0 + 39) * 2)],   ax	    ; row 0, col 39。

    mov             ax,     1301h
    mov             bx,     000fh 
    mov             dx,     0300h
    mov             cx,     14
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     LoadKernelMessage
    int             10h

    mov             ax,     0b800h                      ; the address that displays char on screen; device register
    mov             gs,     ax
    mov             ah,     0fh                         ; black background; white foreground
    mov             al,     '.'
    mov             [gs:((80 * 3 + 14) * 2)],   ax      ; row 3, col 14

KillMotor:                                              ; shut the motor of floppy
    push            dx
    mov             dx,     03f2h
    mov             al,     0
    out             dx,     al
    pop             dx

;============== get momery address size type

	mov	            ax,	    1301h
	mov	            bx,	    000Fh
	mov	            dx,	    0400h		                ;row 4
	mov	            cx,	    24
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    StartGetMemStructMessage
	int	            10h

    mov             ebx,    0
    mov             ax,     0x00
    mov             es,     ax
    mov             di,     MemoryStructBufferAddr      ; with kernel loaded, 0x7e00 is used for memory struct

Label_Get_Mem_Struct:
    mov             eax,    0x0e820
    mov             ecx,    20
    mov             edx,    0x534d4150
    int             15h
    jc              Label_Get_Mem_Fail
    add             di,     20

    cmp             ebx,    0
    jne             Label_Get_Mem_Struct
    jmp             Label_Get_Mem_OK

Label_Get_Mem_Fail:
    mov             ax,     1301h
    mov             bx,     008ch
    mov             dx,     0500h                       ;row 5
    mov             cx,     23
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     GetMemStructErrMessage
    int             10h

    jmp             $

Label_Get_Mem_OK:
    mov             ax,     1301h
    mov             bx,     000fh 
    mov             dx,     0600h                       ;row 6
    mov             cx,     29
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     GetMemStructOKMessage
    int             10h

;==============	get SVGA information

	mov	            ax,	    1301h
	mov	            bx,	    000fh
	mov	            dx,	    0800h		                ;row 8
	mov	            cx,	    23
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    StartGetSVGAVBEInfoMessage
	int	            10h

	mov	            ax,	    0x00
	mov	            es,	    ax
	mov	            di,	    0x8000
	mov	            ax,	    4f00h

	int	            10h

	cmp	            ax,	    004fh

	jz	            .KO

;==============	Fail

	mov	            ax,	    1301h
	mov	            bx,	    008ch
	mov	            dx,	    0900h		                ;row 9
	mov	            cx,	    23
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    GetSVGAVBEInfoErrMessage
	int	            10h

	jmp	            $

.KO:
	mov	            ax,	    1301h
	mov	            bx,	    000fh
	mov	            dx,	    0a00h		                ;row 10
	mov	            cx,	    29
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    GetSVGAVBEInfoOKMessage
	int	            10h

;==============	Get SVGA Mode Info

	mov	            ax,	    1301h
	mov	            bx,	    000fh
	mov	            dx,	    0c00h		                ;row 12
	mov	            cx,	    24
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    StartGetSVGAModeInfoMessage
	int	            10h

	mov	            ax,	    0x00
	mov	            es,	    ax
	mov	            si,	    0x800e

	mov	            esi,	dword	[es:si]
	mov	            edi,	0x8200

Label_SVGA_Mode_Info_Get:
	mov	            cx,	    word    [es:esi]

;==============	display SVGA mode information

	push	        ax
	
	mov	            ax,	    00h
	mov	            al,	    ch
	call	        Label_DispAL

	mov	            ax,	    00h
	mov	            al,	    cl	
	call	        Label_DispAL
	
	pop	            ax

;==============
	
	cmp	            cx,	    0ffffh
	jz	            Label_SVGA_Mode_Info_Finish

	mov	            ax,	    4f01h
	int	            10h

	cmp	            ax,	    004fh

	jnz	            Label_SVGA_Mode_Info_FAIL	

	add	            esi,	2
	add	            edi,	0x100

	jmp	            Label_SVGA_Mode_Info_Get

Label_SVGA_Mode_Info_FAIL:
	mov	            ax,	    1301h
	mov	            bx,	    008ch
	mov	            dx,	    0d00h		                ;row 13
	mov	            cx,	    24
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    GetSVGAModeInfoErrMessage
	int	            10h

Label_SET_SVGA_Mode_VESA_VBE_FAIL:
	jmp	            $

Label_SVGA_Mode_Info_Finish:
	mov	            ax,	    1301h
	mov	            bx,	    000fh
	mov	            dx,	    0e00h		                ;row 14
	mov	            cx,	    30
	push	        ax
	mov	            ax,	    ds
	mov	            es,	    ax
	pop	            ax
	mov	            bp,	    GetSVGAModeInfoOKMessage
	int	            10h

;============== set the SVGA mode(VESA VBE)

    mov             ax,     4f02h
    mov             bx,     4180h                       ; mode0x180, 1440 rows, 900 cols, 32 bits addr=e0000000h, width=32bit
                                                        ; mode0x143, 800 rows, 600 cols, 32 bits addr=e0000000h, width=32bit
    int             10h

    cmp             ax,     004fh
    jnz             Label_SET_SVGA_Mode_VESA_VBE_FAIL

;============== init IDT GDT goto protect mode

    cli                                                 ; cannot trigger BIOS int any more from here

    db              0x66
    lgdt            [GdtPtr]

    ;db              0x66
    ;lidt            [IDT_POINTER]

    mov             eax,    cr0
    or              eax,    1
    mov             cr0,    eax

    jmp             dword   SelectorCode32:Go_To_TMP_Protect


[SECTION .s32]
[BITS 32]

;============== go to tmp long mode

Go_To_TMP_Protect:
    mov             ax,     0x10
    mov             ds,     ax
    mov             es,     ax
    mov             fs,     ax
    mov             ss,     ax
    mov             esp,    7e00h

    call            support_long_mode
    test            eax,    eax

    jz              no_support

;============== init template page table 0x90000

	mov	            dword	[0x90000],  0x91007
	mov	            dword	[0x90004],	0x00000
	mov	            dword	[0x90800],	0x91007
	mov	            dword	[0x90804],	0x00000

	mov	            dword	[0x91000],	0x92007
	mov	            dword	[0x91004],	0x00000

	mov	            dword	[0x92000],	0x000083
	mov	            dword	[0x92004],	0x000000

	mov	            dword	[0x92008],	0x200083
	mov	            dword	[0x9200c],	0x000000

	mov	            dword	[0x92010],	0x400083
	mov	            dword	[0x92014],	0x000000

	mov	            dword	[0x92018],	0x600083
	mov	            dword	[0x9201c],	0x000000

	mov	            dword	[0x92020],	0x800083
	mov	            dword	[0x92024],	0x000000

	mov	            dword	[0x92028],	0xa00083
	mov	            dword	[0x9202c],	0x000000

;============== load GDTR

    db              0x66
    lgdt            [GdtPtr64]
    mov             ax,     0x10
    mov             ds,     ax
    mov             es,     ax
    mov             fs,     ax
    mov             gs,     ax
    mov             ss,     ax

    mov             esp,    7e00h

;============== open PAE

    mov             eax,    cr4
    bts             eax,    5
    mov             cr4,    eax

;============== load cr3

    mov             eax,    0x90000
    mov             cr3,    eax

;============== enable long-mode

    mov             ecx,    0c0000080h                  ;IA32_EFER
    rdmsr

    bts             eax,    8
    wrmsr

;============== open PE and paging

    mov             eax,    cr0
    bts             eax,    0
    bts             eax,    31
    mov             cr0,    eax

    jmp             SelectorCode64:OffsetOfKernelFile

;==============	test support long mode or not

support_long_mode:
    mov             eax,    0x80000000
    cpuid
    cmp             eax,    0x80000001
    setnb           al
    jb              support_long_mode_done
    mov             eax,    0x80000001
    cpuid
    bt              edx,    29
    setc            al

support_long_mode_done:
    movzx           eax,    al
    ret

;============== no support

no_support:
    jmp             $


[SECTION .s16lib]
[BITS 16]

;============== read one sector from disk

Func_ReadOneSector:                                     ; ax, LBA_SectorNum; cl, sectors will be read; es:bx, pointer of read buffer
    push	        bp
	mov	            bp,	    sp
	sub	            esp,	2
	mov	            byte	[bp - 2],	cl              ; bp - 2, the pointer of stack
	push	        bx
	mov	            bl,	    [BPB_SecPerTrk]
	div	            bl                                  ; LBA_SectorNum / BPB_SecPerTrk = Q = al; LBA_SectorNum % BPB_SecPerTrk = R = ah
                                                        ; CylinderNum = Q >> 1; HeadNum = Q & 1; SectorNum = R + 1
	inc	            ah                                  ; SectorNum = ah + 1
	mov	            cl,	    ah                          ; cl, bit0~5 = SectorNum
	mov	            dh,	    al                          ; dh, HeadNum = al & 1
	shr	            al,	    1
	mov	            ch,	    al                          ; ch, CylinderNum = al >> 1
	and	            dh,	    1
	pop	            bx
	mov	            dl,	    [BS_DrvNum]                 ; dl, drive number

Label_Go_On_Reading:
    mov             ah,     2                           ; ah=02h, read disk sector
    mov             al,     byte    [bp - 2]            ; al, sectors will be read = [bp - 2]
    int             13h
    jc              Label_Go_On_Reading
    add             esp,    2
    pop             bp
    ret

;============== get FAT entry

Func_GetFATEntry:                                       ; ax, the current entry; from where get the next entry
	push	        es
	push	        bx
	push	        ax
	mov	            ax,	    00
	mov	            es,	    ax
	pop 	        ax
	mov	            byte	[Odd],  0
	mov	            bx,	    3
	mul	            bx
	mov	            bx,	    2
	div	            bx
	cmp	            dx,	    0
	jz	            Label_Even
	mov	            byte    [Odd],	1

Label_Even:
	xor	            dx,	    dx
	mov	            bx,	    [BPB_BytesPerSec]
	div	            bx
	push	        dx
	mov	            bx,     8000h
	add	            ax,	    SectorNumOfFAT1Start
	mov	            cl,	    2
	call	        Func_ReadOneSector                  ; FAT may cross two sectors
	
	pop	            dx
	add	            bx,	    dx
	mov	            ax,	    [es:bx]
	cmp	            byte	[Odd],  1
	jnz	            Label_Even_2
	shr	            ax,	    4

Label_Even_2:
	and	            ax,	    0fffh
	pop	            bx
	pop	            es
	ret

;==============	display num in al

Label_DispAL:
	push	        ecx
	push	        edx
	push	        edi
	
	mov	            edi,    [DisplayPosition]
	mov	            ah,	    0fh
	mov	            dl,	    al
	shr	            al,	    4
	mov	            ecx,	2

.begin:
	and	            al,	    0fh
	cmp	            al,	    9
	ja	            .1
	add	            al,	    '0'
	jmp	            .2

.1:
	sub	            al,	    0ah
	add	            al,	    'A'

.2:
	mov	            [gs:edi],   ax
	add	            edi,    2
	
	mov	            al,	    dl
	loop	        .begin

	mov	            [DisplayPosition],  edi

	pop	            edi
	pop	            edx
	pop	            ecx
	
	ret


;============== tmp IDT

IDT:
    times           0x50    dq      0
IDT_END:

IDT_POINTER:        dw      IDT_END - IDT - 1
                    dd      IDT

;============== tmp variables

RootDirSizeForLoop	dw      RootDirSectors
SectorNo		    dw      0
Odd			        db	    0
OffsetOfKernelFileCount     dd      OffsetOfKernelFile

DisplayPosition		dd	    0

;============== display messages

StartLoaderMessage: db      "Start Loader"
LoadKernelMessage:  db      "Load Kernel OK"
NoKernelMessage:    db      "ERROR: No Kernel Found"
KernelFileName:     db      "KERNEL  BIN",0
StartGetMemStructMessage:	db	"Start Get Memory Struct."
GetMemStructOKMessage:      db      "Get Memory Struct OK."
GetMemStructErrMessage:     db      "ERROR: Fail to Get Memory Struct"

StartGetSVGAVBEInfoMessage:	db	"Start Get SVGA VBE Info"
GetSVGAVBEInfoErrMessage:	db	"Get SVGA VBE Info ERROR"
GetSVGAVBEInfoOKMessage:	db	"Get SVGA VBE Info SUCCESSFUL!"

StartGetSVGAModeInfoMessage:	db	"Start Get SVGA Mode Info"
GetSVGAModeInfoErrMessage:	db	"Get SVGA Mode Info ERROR"
GetSVGAModeInfoOKMessage:	db	"Get SVGA Mode Info SUCCESSFUL!"