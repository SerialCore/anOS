#include "kprintf.h"
#include "frame.h"
#include "lib.h"

#include <stdarg.h>


struct charactor
{
	// current position of char
    int X;
    int Y;

	// size of char pixel
    int Width;
    int Height;
}Chr;

char buffer[4096] = {0};

// string to num
int skip_atoi(const char** s);

// num to string
static char* number(char* str, long num, int base, int size, int precision,	int type);

// decode formatted string, return length
int vsprintf(char* buffer, const char* format, va_list args);


void set_kprintf_position(int x, int y)
{
    Chr.X = x;
	Chr.Y = y;
}

void set_kprintf_charsize(int x, int y)
{
    Chr.Width = x;
	Chr.Height = y;
}

void kprintf_clear()
{
	frame_clear();
	Chr.X = 0;
	Chr.Y = 0;
}

void kprintf_clear_onerow()
{
	int spaces = get_frame_width() / Chr.Width;
	for (int i = 0; i < spaces; i++)
	{
		kprintf_color32_char(' ', 0, 0);
	}

	/* the os crashs down when
	int frame_width = get_frame_width();
	long length = frame_width * Chr.Height;
	pixel_color32_t metapixel = {0,0,0};
	pixel_color32_t pixels[length];

	for (int i = 0; i < Chr.Height; i++)
	{
		for (int j = 0; j < frame_width; j++)
		{
			metapixel.X = j;
			metapixel.Y = Chr.Y * Chr.Height + i;
			metapixel.Color = 0;
			pixels[i * frame_width + j] = metapixel;
		}
	}

	frame_print_pixel32(pixels, length);
	*/
}

void kprintf_color32_char(unsigned char ch, unsigned int foreground, unsigned int background)
{
	unsigned char* font = font_ascii[ch];

	long length = Chr.Width * Chr.Height;
	pixel_color32_t metapixel = {0,0,0};
	pixel_color32_t pixels[length];

	int testval = 0;
	for (int i = 0; i < Chr.Height; i++)
	{
		testval = 0x100;
		for (int j = 0; j < Chr.Width; j++)		
		{
			// use row and col to determine the pixel position (address) of frame
			metapixel.X = Chr.X * Chr.Width + j;
			metapixel.Y = Chr.Y * Chr.Height + i;

			// scan every bit of font
			testval = testval >> 1;
			if (*font & testval)
				metapixel.Color = foreground;
			else
				metapixel.Color = background;

			pixels[i * Chr.Width + j] = metapixel;
		}
		font++;
	}

	frame_print_pixel32(pixels, length);
}

void kprintf_color32_string(char* str, unsigned int foreground, unsigned int background)
{
	int frame_width = get_frame_width();
	int frame_height = get_frame_height();

    while (*str)
    {
        if (*str == '\r')
		{
			Chr.X = 0;
		}
		else if (*str == '\n')
		{
			// jump to the next row
			Chr.X = 0;
			Chr.Y++;
			kprintf_clear_onerow();
		}
		else if (*str == '\b')
		{
			Chr.X--;
			if (Chr.X < 0)
			{
				Chr.X = (frame_width / Chr.Width - 1) * Chr.Width;
				Chr.Y--;
				if(Chr.Y < 0)
					Chr.Y = (frame_height / Chr.Height - 1) * Chr.Height;
			}
			kprintf_color32_char(' ', foreground, background);
		}
		else if (*str == '\t')
		{
			int tab = (Chr.X / 8 + 1) * 8;
			Chr.X = tab;
		}
		else
		{
			kprintf_color32_char(*str, foreground, background);
			Chr.X++;
		}
		str++;

		// x overscan
		if(Chr.X >= (frame_width / Chr.Width))
		{
			Chr.X = 0;
			Chr.Y++;
			kprintf_clear_onerow();
		}
		// y overscan
		if(Chr.Y >= (frame_height / Chr.Height))
		{
			kprintf_clear();
		}
    }
}

char* kprintf_tostring(char* format, ...)
{
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return buffer;
}

void kprintf_color32(unsigned int foreground, unsigned int background, char* format, ...)
{
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	kprintf_color32_string(buffer, foreground, background);
}

void kprintf(char* format, ...)
{
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	kprintf_color32_string(buffer, White, Black);
}

int skip_atoi(const char** s)
{
	int i = 0;

	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

static char* number(char* str, long num, int base, int size, int precision,	int type)
{
	char c, sign, tmp[50];
	const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type & SMALL) digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (type & LEFT) type &= ~ZEROPAD;
	if (base < 2 || base > 36) return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;
	sign = 0;
	if (type & SIGN && num < 0)
	{
		sign = '-';
		num = -num;
	}
	else
		sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
	if (sign) size--;
	if (type & SPECIAL)
		if (base == 16) size -= 2;
		else if (base == 8) size--;
	i = 0;
	if (num == 0)
		tmp[i++] = '0';
	else while (num != 0)
		tmp[i++] = digits[do_div(num, base)];
	if (i > precision) precision=i;
	size -= precision;
	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL)
		if (base == 8)
			*str++ = '0';
		else if (base == 16) 
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;

	while (i < precision--)
		*str++ = '0';
	while (i-- > 0)
		*str++ = tmp[i];
	while (size-- > 0)
		*str++ = ' ';
	return str;
}

int vsprintf(char* buffer, const char* format, va_list args)
{
	char* str;
	char* s;
	int flags;
	int field_width;
	int precision;
	int len,i;

	int qualifier;		// 'h', 'l', 'L' or 'Z' for integer fields

	for (str = buffer; *format; format++)
	{
		if(*format != '%')
		{
			*str++ = *format;
			continue;
		}
		flags = 0;
		repeat:
			format++;
			switch (*format)
			{
				case '-':flags |= LEFT;	
				goto repeat;
				case '+':flags |= PLUS;	
				goto repeat;
				case ' ':flags |= SPACE;	
				goto repeat;
				case '#':flags |= SPECIAL;	
				goto repeat;
				case '0':flags |= ZEROPAD;	
				goto repeat;
			}

			// get field width
			field_width = -1;
			if (is_digit(*format))
				field_width = skip_atoi(&format);
			else if (*format == '*')
			{
				format++;
				field_width = va_arg(args, int);
				if (field_width < 0)
				{
					field_width = -field_width;
					flags |= LEFT;
				}
			}
			
			// get the precision
			precision = -1;
			if (*format == '.')
			{
				format++;
				if (is_digit(*format))
					precision = skip_atoi(&format);
				else if (*format == '*')
				{	
					format++;
					precision = va_arg(args, int);
				}
				if (precision < 0)
					precision = 0;
			}
			
			qualifier = -1;
			if (*format == 'h' || *format == 'l' || *format == 'L' || *format == 'Z')
			{	
				qualifier = *format;
				format++;
			}
							
			switch (*format)
			{
				case 'c':
					if (!(flags & LEFT))
						while (--field_width > 0)
							*str++ = ' ';
					*str++ = (unsigned char)va_arg(args, int);
					while (--field_width > 0)
						*str++ = ' ';
					break;

				case 's':
					s = va_arg(args,char *);
					if (!s)
						s = '\0';
					len = str_len(s);
					if (precision < 0)
						precision = len;
					else if (len > precision)
						len = precision;
					if (!(flags & LEFT))
						while (len < field_width--)
							*str++ = ' ';
					for (i = 0; i < len ;i++)
						*str++ = *s++;
					while (len < field_width--)
						*str++ = ' ';
					break;

				case 'o':
					if (qualifier == 'l')
						str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
					else
						str = number(str, va_arg(args, unsigned int), 8, field_width, precision, flags);
					break;

				case 'p':
					if (field_width == -1)
					{
						field_width = 2 * sizeof(void *);
						flags |= ZEROPAD;
					}

					str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
					break;

				case 'x':
					flags |= SMALL;

				case 'X':
					if (qualifier == 'l')
						str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
					else
						str = number(str, va_arg(args,unsigned int), 16, field_width, precision, flags);
					break;

				case 'd':
				case 'i':
					flags |= SIGN;
				
				case 'u':
					if (qualifier == 'l')
						str = number(str, va_arg(args, unsigned long), 10, field_width, precision, flags);
					else
						str = number(str, va_arg(args,unsigned int), 10, field_width, precision, flags);
					break;

				case 'n':
					if (qualifier == 'l')
					{
						long *ip = va_arg(args, long *);
						*ip = (str - buffer);
					}
					else
					{
						int *ip = va_arg(args, int *);
						*ip = (str - buffer);
					}
					break;

				case '%':
					*str++ = '%';
					break;

				default:
					*str++ = '%';	
					if (*format)
						*str++ = *format;
					else
						format--;
					break;
			}
	}

	*str = '\0';
	return str - buffer;
}

int str_len(char* String)
{
	register int __res;
	__asm__	__volatile__ (	"cld	\n\t"
					        "repne	\n\t"
					        "scasb	\n\t"
					        "notl	%0	\n\t"
					        "decl	%0	\n\t"
					        :"=c"(__res)
					        :"D"(String),"a"(0),"0"(0xffffffff)
					        :
				        );
	return __res;
}