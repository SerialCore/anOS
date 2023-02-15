#ifndef __PRINTK_H__
#define __PRINTK_H__


typedef enum color
{
    WHITE           = 0x00FFFFFF,
    BLACK           = 0x00000000,
    SilveryWhite    = 0x00D1C9D3,
    Gray            = 0x00A7A9AC,
    DeepGray        = 0x00808285,
    DeeperGray      = 0x0058595B,

    Magenta         = 0x00B31564,
    Red             = 0x00E61B1B,
    RedOrange       = 0x00FF5500,
    Orange          = 0x00FFAA00,
    Gold            = 0x00FFCE00,
    Yellow          = 0x00FFE600,

    GrassGreen      = 0x00A2E61B,
    Green           = 0x0026E600,
    DeepGreen       = 0x00008055,
    Cyan            = 0x0000AACC,
    Blue            = 0x00004CE6,
    IndigoBlue      = 0x003D00B8,

    Violet          = 0x006600CC,
    Purple          = 0x00600080,
    Beige           = 0x00FCE6C9,
    LightBrown      = 0x00BB9167,
    Brown           = 0x008E562E,
    DeepBrown       = 0x00613D30,

    LightPink       = 0x00FF80FF,
    PinkOrange      = 0x00FFC680,
    PinkYellow      = 0x00FFFF80,
    PinkGreen       = 0x0080FF9E,
    PinkBlue        = 0x0080D6FF,
    PinkPurple      = 0x00BCB3FF,
}color_t;


void set_frame_resolution(int x, int y);
void set_frame_position(int x, int y);
void set_frame_charsize(int x, int y);
void set_framebuffer_address(int* addr);
void set_framebuffer_length(unsigned long length);

void frame_clear();
void frame_print_char(unsigned char ch, color_t fore, color_t back);
void frame_print_string(char* string, color_t fore, color_t back);

#endif

/*

#define ZEROPAD	1		// pad with zero
#define SIGN	2		// unsigned/signed long
#define PLUS	4		// show plus
#define SPACE	8		// space if plus
#define LEFT	16		// left justified
#define SPECIAL	32		// 0x
#define SMALL	64		// use 'abcdef' instead of 'ABCDEF'

#define is_digit(c)	((c) >= '0' && (c) <= '9')

#define do_div(n,base) ({ \
int __res; \
__asm__("divq %%rcx":"=a" (n),"=d" (__res):"0" (n),"1" (0),"c" (base)); \
__res; })

*/