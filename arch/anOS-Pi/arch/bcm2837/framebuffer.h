
// PC Screen Font as used by Linux Console
typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned int headersize;
    unsigned int flags;
    unsigned int numglyph;
    unsigned int bytesperglyph;
    unsigned int height;
    unsigned int width;
    unsigned char glyphs;
} __attribute__((packed)) psf_t;

unsigned int _frame_get_width();
unsigned int _frame_get_height();
unsigned int _frame_get_depth();

// detect original hardware resolution and init with dipth 32
void _frameBuffer_detect();

// init framebuffer with specific resolution and dipth
void _frameBuffer_init(unsigned int width, unsigned int height, unsigned int bitDepth);

void _frameBuffer_drawchar(char ch, unsigned int* fontx, unsigned int* fonty);

void _frameBuffer_drawstring(char* string, unsigned int* fontx, unsigned int* fonty);

void _frameBuffer_clearall();

void _frameBuffer_clearrow(unsigned int* fonty);