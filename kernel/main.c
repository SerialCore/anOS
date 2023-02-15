#include "printk.h"

void Start_Kernel(void)
{
	set_frame_resolution(1440, 900);
	set_frame_position(0, 0);
	set_frame_charsize(8, 16);
	set_framebuffer_address((int*)0xffff800000a00000);
	set_framebuffer_length(1440 * 900 * 4);

	frame_print_string("\nHello\tWorld!\n", Yellow, BLACK);

    while(1)
        ;
}