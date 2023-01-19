    org             0x7c00

    %include "fat12.inc"

BaseOfStack         equ     0x7c00
BaseOfLoader        equ     0x1000
OffsetOfLoader      equ     0x00


Label_Start:

    mov             ax,     cs
    mov             ds,     ax
    mov             es,     ax
    mov             ss,     ax
    mov             sp,     BaseOfStack

;============== clear screen
    mov             ax,     0600h                       ; ah=06h, roll the screen; al=00h, clear the screen; al, rows it rolls
    mov             bx,     0700h                       ; the space after rolling
    mov             cx,     0                           ; rolling starts from the left top
    mov             dx,     0184fh                      ; rolling ends with the right bottom
    int             10h

;============== set focus
    mov             ax,     0200h                       ; ah=02h, set the focus
    mov             bx,     0000h                       ; bh, the page number
    mov             dx,     0000h                       ; dh, the col of focus; dl, the row of focus
    int             10h

;============== display on screen: Start Boot
    mov             ax,     1301h                       ; ah=13h, display a string; al=01h, delivery the data in byte, and set focus behind
    mov             bx,     000fh                       ; bh, the page number; bl, the properties and colors of char
    mov             dx,     0000h                       ; dh, the row of focus; dl, the col of focus; the focus before displaying
    mov             cx,     10                          ; the length of message string
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     StartBootMessage
    int             10h

;============== reset disk
    mov             ax,     0000h                       ; ah=00h, reset disk
    mov             dx,     0000h                       ; dl=80h, choose the first hard disk; dl=00h, choose the first floppy
    int             13h

;============== search loader.bin
    mov             word    [SectorNo],    SectorNumOfRootDirStart

Label_Search_In_Root_Dir_Begin:

    cmp             word    [RootDirSizeForLoop],   0
    jz              Label_No_LoaderBin
    dec             word    [RootDirSizeForLoop]
    mov             ax,     00h
    mov             es,     ax
    mov             bx,     8000h                       ; es:bx, pointer of read buffer
    mov             ax,     [SectorNo]                  ; ax=19; SectorNo, pointer of the tmp variable
    mov             cl,     1
    call            Func_ReadOneSector
    mov             si,     LoaderFileName
    mov             di,     8000h
    cld
    mov             dx,     10h

Label_Search_For_LoaderBin:

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
    mov             si,     LoaderFileName
    jmp             Label_Search_For_LoaderBin

Label_Goto_Next_Sector_In_Root_Dir:

    add             word    [SectorNo],     1
    jmp             Label_Search_In_Root_Dir_Begin

;============== read one sector from disk
Func_ReadOneSector:                                     ; ax, LBA_SectorNum; cl, sectors will be read; es:bx, pointer of read buffer

    push            bp
    mov             bp,     sp
    sub             esp,    2
    mov             byte    [bp - 2],   cl              ; bp - 2, the pointer of stack
    push            bx
    mov             bl,     [BPB_SecPerTrk]
    div             bl                                  ; LBA_SectorNum / BPB_SecPerTrk = Q = al; LBA_SectorNum % BPB_SecPerTrk = R = ah
                                                        ; CylinderNum = Q >> 1; HeadNum = Q & 1; SectorNum = R + 1
    inc             ah                                  ; SectorNum = ah + 1
    mov             cl,     ah                          ; cl, bit0~5 = SectorNum
    mov             dh,     al
    and             dh,     1                           ; dh, HeadNum = al & 1
    shr             al,     1
    mov             ch,     al                          ; ch, CylinderNum = al >> 1
    pop             bx
    mov             dl,     [BS_DrvNum]                 ; dl, drive number

Label_Go_On_Reading:

    mov             ah,     2                           ; ah=02h, read disk sector
    mov             al,     byte        [bp - 2]        ; al, sectors will be read = [bp - 2]
    int             13h
    jc              Label_Go_On_Reading
    add             esp,    2
    pop             bp
    ret

;============== get FAT entry
Func_GetFATEntry:                                       ; ax, the current entry; from where get the next entry

    push            es
    push            bx
    push            ax
    mov             ax,     00
    mov             es,     ax
    pop             ax
    mov             byte    [Odd],      0
    mov             bx,     3
    mul             bx
    mov             bx,     2
    div             bx
    cmp             dx,     0
    jz              Label_Even
    mov             byte    [Odd],      1

Label_Even:

    xor             dx,     dx
    mov             bx,     [BPB_BytesPerSec]
    div             bx
    push            dx
    mov             bx,     8000h
    add             ax,     SectorNumOfFAT1Start
    mov             cl,     2
    call            Func_ReadOneSector                  ; FAT may cross two sectors

    pop             dx
    add             bx,     dx
    mov             ax,     [es:bx]
    cmp             byte    [Odd],      1
    jnz             Label_Even_2
    shr             ax,     4

Label_Even_2:

    and             ax,     0fffh
    pop             bx
    pop             es
    ret

;============== found loader.bin name in root director struct
Label_FileName_Found:

    mov             ax,     RootDirSectors
    and             di,     0ffe0h
    add             di,     01ah
    mov             cx,     word    [es:di]
    push            cx
    add             cx,     ax
    add             cx,     SectorBalance               ; consruct the LBA_SectorNum
    mov             ax,     BaseOfLoader
    mov             es,     ax
    mov             bx,     OffsetOfLoader              ; set read buffer to BaseOfLoader:OffsetOfLoader
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
    call            Func_GetFATEntry
    cmp             ax,     0fffh                       ; FATEntry=0fffh, the end of the file
    jz              Label_File_Loaded
    push            ax
    mov             dx,     RootDirSectors
    add             ax,     dx
    add             ax,     SectorBalance
    add             bx,     [BPB_BytesPerSec]
    jmp             Label_Go_On_Loading_File

;============== display on screen: ERROR: No Loader Found
Label_No_LoaderBin:

    mov             ax,     1301h
    mov             bx,     008ch 
    mov             dx,     0100h
    mov             cx,     22
    push            ax
    mov             ax,     ds
    mov             es,     ax
    pop             ax
    mov             bp,     NoLoaderMessage
    int             10h

    jmp             $                                   ; jump to the current code; deadly loop


Label_File_Loaded:

    jmp             BaseOfLoader:OffsetOfLoader

;============== tmp variables
RootDirSizeForLoop  dw      RootDirSectors              ; variable label, the pointer of variable
SectorNo            dw      0
Odd                 db      0

;============== display messages
StartBootMessage:   db      "Start Boot"
NoLoaderMessage:    db      "ERROR: No Loader Found"
LoaderFileName:     db      "LOADER  BIN",0             ; the dot '.' between "loader" and "bin" is replaced by "  "; length must be 11 for FAT12

;============== fill zero till the boot flag
    times           510 - ($ - $$)  db  0               ; $-$$, the length of code; 510-($-$$) the left length in this block of disk
    dw              0xaa55                              ; data in the unit of word; 0xaa is higher than 0x55; this block ends with 0x55 and 0xaa