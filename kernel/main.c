#include "lib.h"
#include "frame.h"
#include "kprintf.h"
#include "console.h"

void Start_Kernel(void)
{
	framebuffer_init();

	console_init();

	kprintf_color32_string("\n[===] anOS (an operating system) preaches:\n", Yellow, Black);
	kprintf_color32_string("\nInspired by human science and religion, we are aiming to create a brand new conscious framework without taking any simulation of existing creatures.\n", Yellow, Black);
	kprintf_color32_string("\nLike that's all of the approprate conditions make fire alive.\n\n", Yellow, Black);

	console_write("Begin:\n\n");
	console_newline();

	kprintf("sizeof(int)= %d * 8", sizeof(int));

    while(1)
        ;
}