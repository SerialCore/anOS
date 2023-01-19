RootDirSectors              equ     14                  ; (BPB_RootEntCnt * 32 + BPB_BytesPerSec - 1) / BPB_BytesPerSec
SectorNumOfRootDirStart     equ     19                  ; BPB_RsvdSecCnt + BPB_FATSz16 * BPB_NumFATs
SectorNumOfFAT1Start        equ     1                   ; FAT1 starts with sector 1
SectorBalance               equ     17                  ; SectorNumOfRootDirStart - 2    

    jmp             short   Label_Start
    nop
    BS_OEMName      db      'anOS    '
    BPB_BytesPerSec dw      512
    BPB_SecPerClus  db      1
    BPB_RsvdSecCnt  dw      1
    BPB_NumFATs     db      2
    BPB_RootEntCnt  dw      224
    BPB_TotSec16    dw      2880
    BPB_Media       db      0xf0
    BPB_FATSz16     dw      9
    BPB_SecPerTrk   dw      18
    BPB_NumHeads    dw      2
    BPB_HiddSec     dd      0
    BPB_TotSec32    dd      0
    BS_DrvNum       db      0x00                        ; drive number; bit7=1 for disk; bit7=0 for floppy
    BS_Reserved1    db      0
    BS_BootSig      db      29h
    BS_VolID        dd      0
    BS_VolLab       db      'bootloader '
    BS_FileSysType  db      'FAT12   '