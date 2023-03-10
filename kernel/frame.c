#include "frame.h"


struct frameinfo
{
    int Width;					// x resolution
    int Height;					// y resolution
	int Deep;					// bits of color

	int LastX;					// the last position of pixel x
	int LastY;					// the last position of pixel y

    unsigned int* FB_addr;		// framebuffer address
    unsigned long FB_length;	// int bytes
}Fri;


void framebuffer_init()
{
	Fri.Width = 1440;
	Fri.Height = 900;
	Fri.Deep = 32;
	Fri.FB_addr = (int*)0xffff800000a00000;
	Fri.FB_length = Fri.Width * Fri.Height * (Fri.Deep / 8);
}

int get_frame_width()
{
	return Fri.Width;
}

int get_frame_height()
{
	return Fri.Height;
}

int get_frame_deep()
{
	return Fri.Deep;
}

int get_frame_lastx()
{
	return Fri.LastX;
}

int get_frame_lasty()
{
	return Fri.LastY;
}

void frame_print_pixel32(pixel_color32_t* pixels, long length)
{
	unsigned int* addr = Fri.FB_addr;

	for (long i = 0; i < length; i++)
	{
		*(addr + pixels[i].Y * Fri.Width + pixels[i].X) = pixels[i].Color;
	}

	Fri.LastX = pixels[length - 1].X;
	Fri.LastY = pixels[length - 1].Y;
}

void frame_clear()
{
	unsigned char* addr = (char*)Fri.FB_addr;

	for (long i = 0; i < Fri.FB_length; i++)
	{
		*(addr + i) = 0;
	}

	Fri.LastX = Fri.Width;
	Fri.LastY = Fri.Height;
}