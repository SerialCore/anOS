#include <arch/bcm2837/framebuffer.h>
#include <arch/bcm2837/mailbox.h>
#include <lib/math/basicmath.h>

#define _is_x_overscan_     (x * (font->width + 1) > (currentWidth - 22 * font->width))
#define _is_y_overscan_     (y * font->height > (currentHeight - font->height))

// be carefull about the directory
extern volatile unsigned char _binary_build_font_psf_start;

// screen settings
unsigned int currentWidth = 0, currentHeight = 0, currentDepth = 0, currentPitch = 0;

// FrameBufferInfo.pointer
unsigned char* lfb = 0;

unsigned int _frame_get_width() { return currentWidth; }

unsigned int _frame_get_height() { return currentHeight; }

unsigned int _frame_get_depth() { return currentDepth; }

void _frameBuffer_detect()
{
    mailbuffer[0] = 12 * 4;
    mailbuffer[1] = MBOX_REQUEST;

    mailbuffer[2] = MBOX_TAG_Get_PhysicalWH;
    mailbuffer[3] = 8;
    mailbuffer[4] = 8;
    mailbuffer[5] = 0;
    mailbuffer[6] = 0;

    mailbuffer[7] = MBOX_TAG_LAST;

    if (_mailbox_call(MBOX_CH_PROP_ARM2VC))
    {
        currentWidth = mailbuffer[5];
        currentHeight = mailbuffer[6];
        //TODO: if fail, re-detect or use default settings
        _frameBuffer_init(currentWidth, currentHeight, 32);
    }
    else
    {
        _frameBuffer_init(1024, 768, 32);
    }
}

void _frameBuffer_init(unsigned int width, unsigned int height, unsigned int bitDepth)
{
    mailbuffer[0] = 35 * 4;
    mailbuffer[1] = MBOX_REQUEST;

    mailbuffer[2] = MBOX_TAG_Set_PhysicalWH;  //set phy wh
    mailbuffer[3] = 8;
    mailbuffer[4] = 8;
    mailbuffer[5] = width;         //FrameBufferInfo.width
    mailbuffer[6] = height;          //FrameBufferInfo.height

    mailbuffer[7] = MBOX_TAG_Set_VirtualWH;  //set virt wh
    mailbuffer[8] = 8;
    mailbuffer[9] = 8;
    mailbuffer[10] = width;        //FrameBufferInfo.virtual_width
    mailbuffer[11] = height;         //FrameBufferInfo.virtual_height
    
    mailbuffer[12] = MBOX_TAG_Set_VirtualOffset; //set virt offset
    mailbuffer[13] = 8;
    mailbuffer[14] = 8;
    mailbuffer[15] = 0;           //FrameBufferInfo.x_offset
    mailbuffer[16] = 0;           //FrameBufferInfo.y.offset
    
    mailbuffer[17] = MBOX_TAG_Set_Depth; //set depth
    mailbuffer[18] = 4;
    mailbuffer[19] = 4;
    mailbuffer[20] = bitDepth;          //FrameBufferInfo.depth

    mailbuffer[21] = MBOX_TAG_Set_PixelOrder; //set pixel order
    mailbuffer[22] = 4;
    mailbuffer[23] = 4;
    mailbuffer[24] = 1;           //RGB, not BGR preferably

    mailbuffer[25] = MBOX_TAG_AllocateBuffer; //get framebuffer, gets alignment on request
    mailbuffer[26] = 8;
    mailbuffer[27] = 8;
    mailbuffer[28] = 4096;        //FrameBufferInfo.pointer
    mailbuffer[29] = 0;           //FrameBufferInfo.size

    mailbuffer[30] = MBOX_TAG_Get_Pitch; //get pitch
    mailbuffer[31] = 4;
    mailbuffer[32] = 4;
    mailbuffer[33] = 0;           //FrameBufferInfo.pitch

    mailbuffer[34] = MBOX_TAG_LAST;

    if (_mailbox_call(MBOX_CH_PROP_ARM2VC) && mailbuffer[28] != 0)
    {
        mailbuffer[28] &= 0x3FFFFFFF;
        currentWidth = mailbuffer[5];
        currentHeight = mailbuffer[6];
        currentDepth = mailbuffer[20];
        currentPitch = mailbuffer[33];
        lfb = (void*)((unsigned long)mailbuffer[28]);
    }
}

//TODO: set _draw_char as inline and a final entry
void _frameBuffer_drawchar(char ch, unsigned int* fontx, unsigned int* fonty)
{
    unsigned int x = *fontx;
    unsigned int y = *fonty;

    // get our font
    psf_t* font = (psf_t*)&_binary_build_font_psf_start;
    // draw next character if it's not zero
    // get the offset of the glyph. Need to adjust this to support unicode table
    unsigned char *glyph = (unsigned char*)&_binary_build_font_psf_start +
        font->headersize + ((unsigned char)ch < font->numglyph ? ch : 0) * font->bytesperglyph;
    // calculate the offset on screen
    int offs = (y * font->height * currentPitch) + (x * (font->width + 1) * 4);
    // variables
    int i,j, line, mask, bytesperline = (font->width + 7) / 8;
    // handle carrige return
    if (ch == '\r')
    {
        x = 0;
    }
    else if (ch == '\n') // new line
    {
        x = 0; y++;
        if (_is_y_overscan_)
        {
            y = 0;
        }
    }
    else
    {
        // display a character
        for (j = 0; j < font->height; j++)
        {
            // display one row
            line = offs;
            mask = 1 << (font->width - 1);
            for (i = 0; i < font->width; i++)
            {
                // if bit set, we use white color, otherwise black
                *((unsigned int*)(lfb + line)) = ((int)*glyph) & mask ? 0xFFFFFF : 0;
                mask >>= 1;
                line += 4;
            }
            // adjust to next line
            glyph += bytesperline;
            offs += currentPitch;
        }
        x++;
        // checkout overscan
        if (_is_x_overscan_)
        {
            x = 0; y++;
            if (_is_y_overscan_)
            {
                y = 0;
            }
        }
    }

    *fontx = x;
    *fonty = y;
}

void _frameBuffer_drawstring(char* string, unsigned int* fontx, unsigned int* fonty)
{
    unsigned int x = *fontx;
    unsigned int y = *fonty;

    // get our font
    psf_t* font = (psf_t*)&_binary_build_font_psf_start;
    // draw next character if it's not zero
    while (*string)
    {
        // get the offset of the glyph. Need to adjust this to support unicode table
        unsigned char *glyph = (unsigned char*)&_binary_build_font_psf_start +
            font->headersize + (*((unsigned char*)string) < font->numglyph ? *string : 0) * font->bytesperglyph;
        // calculate the offset on screen
        int offs = (y * font->height * currentPitch) + (x * (font->width + 1) * 4);
        // variables
        int i,j, line, mask, bytesperline = (font->width + 7) / 8;
        // handle carrige return
        if (*string == '\r')
        {
            x = 0;
        }
        else if (*string == '\n') // new line
        {
            x = 0; y++;
            if (_is_y_overscan_)
            {
                y = 0;
            }
            _frameBuffer_clearrow(&y);
        }
        else
        {
            // display a character
            for (j = 0; j < font->height; j++)
            {
                // display one row
                line = offs;
                mask = 1 << (font->width - 1);
                for (i = 0; i < font->width; i++)
                {
                    // if bit set, we use white color, otherwise black
                    *((unsigned int*)(lfb + line)) = ((int)*glyph) & mask ? 0xFFFFFF : 0;
                    mask >>= 1;
                    line += 4;
                }
                // adjust to next line
                glyph += bytesperline;
                offs += currentPitch;
            }
            x++;
            // checkout overscan
            if (_is_x_overscan_)
            {
                x = 0; y++;
                if (_is_y_overscan_)
                {
                    y = 0;
                }
                _frameBuffer_clearrow(&y);
            }
        }
        // next character
        string++;
    }

    *fontx = x;
    *fonty = y;
}

void _frameBuffer_clearall()
{
    unsigned int x = 0;
    unsigned int y = 0;

    // get our font
    psf_t* font = (psf_t*)&_binary_build_font_psf_start;
    // draw next character if it's not zero
    while (!_is_y_overscan_)
    {
        // get the offset of the glyph. Need to adjust this to support unicode table
        unsigned char *glyph = (unsigned char*)&_binary_build_font_psf_start +
            font->headersize + (*((unsigned char*)" ") < font->numglyph ? * " " : 0) * font->bytesperglyph;
        // calculate the offset on screen
        int offs = (y * font->height * currentPitch) + (x * (font->width + 1) * 4);
        // variables
        int i,j, line, mask, bytesperline = (font->width + 7) / 8;
        // display a character
        for (j = 0; j < font->height; j++)
        {
            // display one row
            line = offs;
            mask = 1 << (font->width - 1);
            for (i = 0; i < font->width; i++)
            {
                // if bit set, we use white color, otherwise black
                *((unsigned int*)(lfb + line)) = ((int)*glyph) & mask ? 0xFFFFFF : 0;
                mask >>= 1;
                line += 4;
            }
            // adjust to next line
            glyph += bytesperline;
            offs += currentPitch;
        }
        x++;
        // checkout overscan
        if (_is_x_overscan_)
        {
            x = 0; y++;
        }
    }
}

void _frameBuffer_clearrow(unsigned int* fonty)
{
    unsigned int x = 0;
    unsigned int y = *fonty;

    // get our font
    psf_t* font = (psf_t*)&_binary_build_font_psf_start;
    // draw next character if it's not zero
    while (!_is_x_overscan_)
    {
        // get the offset of the glyph. Need to adjust this to support unicode table
        unsigned char *glyph = (unsigned char*)&_binary_build_font_psf_start +
            font->headersize + (*((unsigned char*)" ") < font->numglyph ? * " " : 0) * font->bytesperglyph;
        // calculate the offset on screen
        int offs = (y * font->height * currentPitch) + (x * (font->width + 1) * 4);
        // variables
        int i,j, line, mask, bytesperline = (font->width + 7) / 8;
        // display a character
        for (j = 0; j < font->height; j++)
        {
            // display one row
            line = offs;
            mask = 1 << (font->width - 1);
            for (i = 0; i < font->width; i++)
            {
                // if bit set, we use white color, otherwise black
                *((unsigned int*)(lfb + line)) = ((int)*glyph) & mask ? 0xFFFFFF : 0;
                mask >>= 1;
                line += 4;
            }
            // adjust to next line
            glyph += bytesperline;
            offs += currentPitch;
        }
        x++;
    }
}