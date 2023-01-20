#include <arch/bcm2837/random.h>
#include <lib/math/basicmath.h>

void _rand_init()
{
    *RNG_STATUS = 0x40000;
    // mask interrupt
    *RNG_INT_MASK |= 1;
    // enable
    *RNG_CTRL |= 1;
    // wait for gaining some entropy
    while(!((*RNG_STATUS) >> 24));
}

unsigned int _rand_get(unsigned int min, unsigned int max)
{
    return (*RNG_DATA) % (max - min) + min;
}