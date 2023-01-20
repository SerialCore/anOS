#include <arch/bcm2837/gpio.h>

void _gpio_function(unsigned int pin, gpio_func_t function)
{
    // check pin_num <= 53 and pin_func <= 7
    if (pin > 53 || function > 7)
    {
        return;
    }

    // three bits for each pin, shift = pin * 3
    if (pin < 10)
    {
        *GPFSEL0 = function << (pin * 3);
    }
    else if (pin < 20)
    {
        *GPFSEL1 = function << ((pin - 10) * 3);
    }
    else if (pin < 30)
    {
        *GPFSEL2 = function << ((pin - 20) * 3);
    }
    else if (pin < 40)
    {
        *GPFSEL3 = function << ((pin - 30) * 3);
    }
    else if (pin < 50)
    {
        *GPFSEL4 = function << ((pin - 40) * 3);
    }
    else
    {
        *GPFSEL5 = function << ((pin - 50) * 3);
    }
}

void _gpio_write(unsigned int pin, unsigned int value)
{
    if (pin > 53)
    {
        return;
    }

    // one bit for each pin, shift = pin % 32
    if (value)
    {
        if (pin < 32)
        {
            *GPSET0 = 1 << pin;
        }
        else
        {
            *GPSET1 = 1 << (pin - 32);
        }
    }
    else
    {
        if (pin < 32)
        {
            *GPCLR0 = 1 << pin;
        }
        else
        {
            *GPCLR1 = 1 << (pin - 32);
        }
    }
}

unsigned int _gpio_read(unsigned int pin)
{
    return 0;
}