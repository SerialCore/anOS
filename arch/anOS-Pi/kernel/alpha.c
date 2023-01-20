#include <kernel/console.h>
#include <arch/bcm2837/timer.h>
#include <arch/bcm2837/random.h>

void alpha()
{
    console_init();
    console_printline("Inspired by human science and religion, we are aiming to create a brand new conscious framework without taking any simulation of existing creatures.");
    console_printline("Like that's all of the approprate conditions make fire alive.");

    _rand_init();
    for (;;)
    {
        _delay_milliseconds(300);
        console_printhex(_rand_get(0x00, 0xFFFFFFFF));
        console_printnew();
    }
    
    for (;;) {}
    
}
