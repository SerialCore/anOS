#include <arch/bcm2837/uart1.h>
#include <arch/bcm2837/gpio.h>

void _init_uart1()
{
    register unsigned int r;

    // initialize UART
    *AUX_ENABLE |= 1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;
    *AUX_MU_LCR = 3;       // 8 bits
    *AUX_MU_MCR = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_IIR = 0xc6;    // disable interrupts
    *AUX_MU_BAUD = 270;    // 115200 baud

    // map UART1 to GPIO pins
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (2 << 12) | (2 << 15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r = 150; while (r--);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    r = 150; while (r--);
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx
}

void _uart1_sendchar(unsigned int c)
{
    // wait until we can send
    while (!(*AUX_MU_LSR & 0x20));
    // write the character to the buffer
    *AUX_MU_IO = c;
}

void _uart1_sendstring(char *s)
{
    while (*s)
    {
        // convert newline to carrige return + newline
        if (*s == '\n')
            _uart1_sendchar('\r');
        _uart1_sendchar(*s++);
    }
}

char _uart1_getchar()
{
    char r;
    // wait until something is in the buffer
    while (!(*AUX_MU_LSR & 0x01));
    // read it and return
    r = (char)(*AUX_MU_IO);
    // convert carrige return to newline
    return r == '\r'? '\n' : r;
}