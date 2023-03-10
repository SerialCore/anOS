#include "console.h"
#include "kprintf.h"


// uesless for now
struct position
{
	// current position of char
    int XPosition;
    int YPosition;

	// size of char pixel
    int XCharSize;
    int YCharSize;
}Pos;

// uesless for now
void console_init()
{
    Pos.XPosition = 0;
    Pos.YPosition = 0;
    Pos.XCharSize = 8;
    Pos.YCharSize = 16;
    set_kprintf_position(0, 0);
    set_kprintf_charsize(8, 16);
}

void console_write(char* str)
{
    kprintf(str);
}

void console_newline()
{
    kprintf("[===] ");
}

void console_clear()
{
    kprintf_clear();
    console_newline();
}

// connect with serial port
