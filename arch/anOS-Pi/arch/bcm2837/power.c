#include <arch/bcm2837/power.h>
#include <arch/bcm2837/gpio.h>
#include <arch/bcm2837/timer.h>
#include <arch/bcm2837/mailbox.h>

void _shutdown()
{
    unsigned long r;

    // power off devices one by one
    for (r = 0; r < 16; r++)
    {
        mailbuffer[0] = 8 * 4;
        mailbuffer[1] = MBOX_REQUEST;
        mailbuffer[2] = MBOX_TAG_SET_PowerState; // set power state
        mailbuffer[3] = 8;
        mailbuffer[4] = 8;
        mailbuffer[5] = (unsigned int)r;   // device id
        mailbuffer[6] = 0;                 // bit 0: off, bit 1: no wait
        mailbuffer[7] = MBOX_TAG_LAST;

        _mailbox_call(MBOX_CH_PROP_ARM2VC);
    }

    // power off gpio pins (but not VCC pins)
    *GPFSEL0 = 0; *GPFSEL1 = 0; *GPFSEL2 = 0; *GPFSEL3 = 0; *GPFSEL4 = 0; *GPFSEL5 = 0; *GPPUD = 0;
    _delay_cycles(150);
    *GPPUDCLK0 = 0xffffffff; *GPPUDCLK1 = 0xffffffff;
    _delay_cycles(150);
    *GPPUDCLK0 = 0; *GPPUDCLK1 = 0;        // flush GPIO setup

    // power off the SoC (GPU + CPU)
    r = *PM_RSTS; r &= ~0xfffffaaa;
    r |= 0x555;    // partition 63 used to indicate halt
    *PM_RSTS = PM_WDOG_MAGIC | r;
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

void _reboot()
{
    unsigned int r;

    // trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS; r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;   // boot from partition 0
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}