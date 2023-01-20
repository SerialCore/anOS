
// address
#define POWER               (volatile unsigned int*) 0x3F100000
#define PM_RSTC             (volatile unsigned int*) 0x3F10001c
#define PM_RSTS             (volatile unsigned int*) 0x3F100020
#define PM_WDOG             (volatile unsigned int*) 0x3F100024
#define PM_WDOG_MAGIC       0x5a000000
#define PM_RSTC_FULLRST     0x00000020

void _shutdown();

void _reboot();