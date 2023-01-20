#include <arch/bcm2837/gpio.h>
#include <arch/bcm2837/uart0.h>
#include <arch/bcm2837/mailbox.h>

void _init_uart0()
{
    register unsigned int r;

    /* initialize UART */
    *UART0_CR = 0;         // turn off UART0

    /* set up clock for consistent divisor values */
    mailbuffer[0] = 9 * 4;
    mailbuffer[1] = MBOX_REQUEST;
    mailbuffer[2] = MBOX_TAG_SET_ClockRate; // set clock rate
    mailbuffer[3] = 12;
    mailbuffer[4] = 8;
    mailbuffer[5] = 2;           // UART clock
    mailbuffer[6] = 4000000;     // 4Mhz
    mailbuffer[7] = 0;           // clear turbo
    mailbuffer[8] = MBOX_TAG_LAST;

    _mailbox_call(MBOX_CH_PROP_ARM2VC);

    // map UART0 to GPIO pins
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r = 150; while (r--);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r=150; while (r--);
    *GPPUDCLK0 = 0;        // flush GPIO setup

    *UART0_ICR = 0x7FF;    // clear interrupts
    *UART0_IBRD = 2;       // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0b11 << 5; // 8n1
    *UART0_CR = 0x301;     // enable Tx, Rx, FIFO
}

void _uart0_sendchar(unsigned int c)
{
    // wait until we can send
    while (*UART0_FR & 0x20);
    // write the character to the buffer
    *UART0_DR = c;
}

void _uart0_sendstring(char *s)
{
    while (*s)
    {
        // convert newline to carrige return + newline
        if (*s == '\n')
            _uart0_sendchar('\r');
        _uart0_sendchar(*s++);
    }
}

// Display a binary value in hexadecimal
void _uart0_sendhex(unsigned int d)
{
    unsigned int n;
    int c;

    for (c = 28; c >= 0; c -= 4)
    {
        // get highest tetrad
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9? 0x37 : 0x30;
        _uart0_sendchar(n);
    }
}

char _uart0_getchar()
{
    char r;
    // wait until something is in the buffer
    while (*UART0_FR & 0x10);
    // read it and return
    r = (char)(*UART0_DR);
    // convert carrige return to newline
    return r == '\r'? '\n' : r;
}
