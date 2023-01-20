#include <arch/bcm2837/mailbox.h>
#include <lib/data/section.h>

volatile unsigned int __aligned(16) mailbuffer[256] = {0};

int _mailbox_call(mailbox_channel_t channel)
{
    unsigned int r = (((unsigned int)((unsigned long)&mailbuffer) & ~0xF) | (channel & 0xF));
    // wait until we can write to the mailbox
    while (*MBOX_STATUS & MBOX_FULL);
    // write the address of our message to the mailbox with channel identifier
    *MBOX_WRITE = r;
    // now wait for the response
    while (1)
    {
        // is there a response?
        while (*MBOX_STATUS & MBOX_EMPTY);
        // is it a response to our message?
        if (r == *MBOX_READ)
            // is it a valid successful response?
            return mailbuffer[1] == MBOX_RESPONSE;
    }
    return 0;
}