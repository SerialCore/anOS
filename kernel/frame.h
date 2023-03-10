#ifndef __FRAME_H__
#define __FRAME_H__


typedef struct pixel_color32
{
    short X;
    short Y;
    unsigned int Color;
}pixel_color32_t;

typedef struct pixel_color64
{
    short X;
    short Y;
    unsigned long Color;
}pixel_color64_t;


void framebuffer_init();

int get_frame_width();
int get_frame_height();
int get_frame_deep();
int get_frame_lastx();
int get_frame_lasty();

void frame_print_pixel32(pixel_color32_t* pixels, long length);
void frame_clear();

#endif