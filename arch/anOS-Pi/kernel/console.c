#include <kernel/console.h>
#include <arch/bcm2837/uart0.h>
#include <arch/bcm2837/uart1.h>
#include <arch/bcm2837/framebuffer.h>

// current position of last char
unsigned int currentX = 0, currentY = 0;

void console_init()
{
    currentX = currentY = 0;
    
    // initialize framebuffer
    _frameBuffer_detect();

    // for screen
    _frameBuffer_drawstring("x->> ", &currentX, &currentY);
    // for ttl and qemu
    _uart0_sendstring("x->> ");
}

void console_printhex(unsigned int d)
{
    unsigned int n;
    int c;

    for (c = 28; c >= 0; c -= 4)
    {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9? 0x37 : 0x30;
        console_printchar(n);
    }
}

void console_printchar(char ch) // will be unsigned int?
{
    // for screen
    _frameBuffer_drawchar(ch, &currentX, &currentY);
    // for ttl and qemu
    _uart0_sendchar(ch);
}

void console_printline(char* str)
{
    // for screen
    _frameBuffer_drawstring(str, &currentX, &currentY);
    _frameBuffer_drawstring("\nx->> ", &currentX, &currentY); //TODO: count as currentY++;
    // for ttl and qemu
    _uart0_sendstring(str);
    _uart0_sendstring("\nx->> ");
}

void console_printnew()
{
    // for screen
    _frameBuffer_drawstring("\nx->> ", &currentX, &currentY); //TODO: count as currentY++;
    // for ttl and qemu
    _uart0_sendstring("\nx->> ");
}

void console_clearall()
{
    // only for screen
    _frameBuffer_clearall();
    currentX = currentY = 0;
}