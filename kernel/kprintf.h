#ifndef __KPRINTF_H__
#define __KPRINTF_H__

#include <stdarg.h>
#include "font.h"


extern unsigned char font_ascii[256][16];

#define ZEROPAD	1		// pad with zero
#define SIGN	2		// unsigned/signed long
#define PLUS	4		// show plus
#define SPACE	8		// space if plus
#define LEFT	16		// left justified
#define SPECIAL	32		// 0x
#define SMALL	64		// use 'abcdef' instead of 'ABCDEF'

#define is_digit(c)	    ((c) >= '0' && (c) <= '9')

#define do_div(n, base) ({ int __res; __asm__("divq %%rcx":"=a" (n),"=d" (__res):"0" (n),"1" (0),"c" (base)); __res; })


void set_kprintf_position(int x, int y);
void set_kprintf_charsize(int x, int y);

void kprintf_clear();
// should be private
void kprintf_clear_onerow();
void kprintf_color32_char(unsigned char ch, unsigned int foreground, unsigned int background);
void kprintf_color32_string(char* str, unsigned int foreground, unsigned int background);

char* kprintf_tostring(char* format, ...);
void kprintf_color32(unsigned int foreground, unsigned int background, char* format, ...);
void kprintf(char* format, ...);

#endif