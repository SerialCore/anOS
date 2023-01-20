#include <arch/bcm2837/timer.h>

unsigned int _getL_time_stamp()
{
    return *SYSTMR_LO;
}

unsigned int _getH_time_stamp()
{
    return *SYSTMR_HI;
}

unsigned long _get_time_stamp()
{
    unsigned int h=-1, l;

    // we must read MMIO area as two separate 32 bit reads
    h = *SYSTMR_HI;
    l = *SYSTMR_LO;

    // we have to repeat it if high word changed during read
    if (h != *SYSTMR_HI)
    {
        h = *SYSTMR_HI;
        l = *SYSTMR_LO;
    }

    // compose long int value
    return ((unsigned long) h << 32) | l;
}

void _delay_microseconds(unsigned int delay)
{
    unsigned long start = _get_time_stamp();

    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if (start)
        while (_get_time_stamp() - start < delay);
}


void _delay_milliseconds(unsigned int delay)
{
    delay *= 1000;
    _delay_microseconds(delay);
}

void _delay_cycles(unsigned int n)
{
    if (n) while(n--);
}