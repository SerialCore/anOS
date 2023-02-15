#include "printk.h"
#include "font.h"


struct position
{
	// resolution of the frame
    int XResolution;
    int YResolution;

	// current position of char
    int XPosition;
    int YPosition;

	// size of char pixel
    int XCharSize;
    int YCharSize;

    unsigned int* FB_addr;
    unsigned long FB_length;
}Pos;

void set_frame_resolution(int x, int y)
{
    Pos.XResolution = x;
	Pos.YResolution = y;
}

void set_frame_position(int x, int y)
{
    Pos.XPosition = x;
	Pos.YPosition = y;
}

void set_frame_charsize(int x, int y)
{
    Pos.XCharSize = x;
	Pos.YCharSize = y;
}

void set_framebuffer_address(int* addr)
{
    Pos.FB_addr = addr;
}

void set_framebuffer_length(unsigned long length)
{
    Pos.FB_length = length;
}

void frame_clear()
{
	unsigned int* addr = Pos.FB_addr;

	for (int i = 0; i < Pos.XResolution * Pos.YResolution; i++)
	{
		*(addr + i) = BLACK;
	}

	Pos.XPosition = 0;
	Pos.YPosition = 0;
}

void frame_print_char(unsigned char ch, color_t fore, color_t back)
{
	unsigned int* addr = 0;
	unsigned char* font = 0;
	font = font_ascii[ch];

	int testval = 0;
	for (int i = 0; i < Pos.YCharSize; i++)
	{
		// use row and col to determine the pixel position (address) of frame
		addr = Pos.FB_addr + Pos.XResolution * (Pos.YPosition * Pos.YCharSize + i) + Pos.XPosition * Pos.XCharSize;
		testval = 0x100;
		for (int j = 0; j < Pos.XCharSize; j++)		
		{
			// scan every bit of font
			testval = testval >> 1;
			if (*font & testval)
				*addr = fore;
			else
				*addr = back;
			addr++;
		}
		font++;		
	}
}

void frame_print_string(char* str, color_t fore, color_t back)
{
    while (*str)
    {
        if (*str == '\r')
		{
			Pos.XPosition = 0;
		}
		else if (*str == '\n')
		{
			// jump to the next row
			Pos.XPosition = 0;
			Pos.YPosition++;
		}
		else if (*str == '\b')
		{
			Pos.XPosition--;
			if (Pos.XPosition < 0)
			{
				Pos.XPosition = (Pos.XResolution / Pos.XCharSize - 1) * Pos.XCharSize;
				Pos.YPosition--;
				if(Pos.YPosition < 0)
					Pos.YPosition = (Pos.YResolution / Pos.YCharSize - 1) * Pos.YCharSize;
			}
			frame_print_char(' ', fore, back);
		}
		else if (*str == '\t')
		{
			// jump to the next 4 cols
			Pos.XPosition+=4;
		}
		else
		{
			frame_print_char(*str, fore, back);
			Pos.XPosition++;
		}
		str++;

		// x overscan
		if(Pos.XPosition >= (Pos.XResolution / Pos.XCharSize))
		{
			Pos.XPosition = 0;
			Pos.YPosition++;
		}
		// y overscan
		if(Pos.YPosition >= (Pos.YResolution / Pos.YCharSize))
		{
			frame_clear();
		}
    }
}

/*

// string to num
int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

// num to string
static char * number(char * str, long num, int base, int size, int precision,	int type)
{
	char c,sign,tmp[50];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type&SMALL) digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (type&LEFT) type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ' ;
	sign = 0;
	if (type&SIGN && num < 0) {
		sign='-';
		num = -num;
	} else
		sign=(type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
	if (sign) size--;
	if (type & SPECIAL)
		if (base == 16) size -= 2;
		else if (base == 8) size--;
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num!=0)
		tmp[i++]=digits[do_div(num,base)];
	if (i > precision) precision=i;
	size -= precision;
	if (!(type & (ZEROPAD + LEFT)))
		while(size-- > 0)
			*str++ = ' ';
	if (sign)
		*str++ = sign;
	if (type & SPECIAL)
		if (base == 8)
			*str++ = '0';
		else if (base==16) 
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	if (!(type & LEFT))
		while(size-- > 0)
			*str++ = c;

	while(i < precision--)
		*str++ = '0';
	while(i-- > 0)
		*str++ = tmp[i];
	while(size-- > 0)
		*str++ = ' ';
	return str;
}

// decode formatted string
int vsprintf(char * buf,const char *fmt, va_list args)
{
	char * str,*s;
	int flags;
	int field_width;
	int precision;
	int len,i;

	int qualifier;		// 'h', 'l', 'L' or 'Z' for integer fields

	for(str = buf; *fmt; fmt++)
	{

		if(*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}
		flags = 0;
		repeat:
			fmt++;
			switch(*fmt)
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
			if(is_digit(*fmt))
				field_width = skip_atoi(&fmt);
			else if(*fmt == '*')
			{
				fmt++;
				field_width = va_arg(args, int);
				if(field_width < 0)
				{
					field_width = -field_width;
					flags |= LEFT;
				}
			}
			
			// get the precision

			precision = -1;
			if(*fmt == '.')
			{
				fmt++;
				if(is_digit(*fmt))
					precision = skip_atoi(&fmt);
				else if(*fmt == '*')
				{	
					fmt++;
					precision = va_arg(args, int);
				}
				if(precision < 0)
					precision = 0;
			}
			
			qualifier = -1;
			if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z')
			{	
				qualifier = *fmt;
				fmt++;
			}
							
			switch(*fmt)
			{
				case 'c':

					if(!(flags & LEFT))
						while(--field_width > 0)
							*str++ = ' ';
					*str++ = (unsigned char)va_arg(args, int);
					while(--field_width > 0)
						*str++ = ' ';
					break;

				case 's':
				
					s = va_arg(args,char *);
					if(!s)
						s = '\0';
					len = strlen(s);
					if(precision < 0)
						precision = len;
					else if(len > precision)
						len = precision;
					
					if(!(flags & LEFT))
						while(len < field_width--)
							*str++ = ' ';
					for(i = 0;i < len ;i++)
						*str++ = *s++;
					while(len < field_width--)
						*str++ = ' ';
					break;

				case 'o':
					
					if(qualifier == 'l')
						str = number(str,va_arg(args,unsigned long),8,field_width,precision,flags);
					else
						str = number(str,va_arg(args,unsigned int),8,field_width,precision,flags);
					break;

				case 'p':

					if(field_width == -1)
					{
						field_width = 2 * sizeof(void *);
						flags |= ZEROPAD;
					}

					str = number(str,(unsigned long)va_arg(args,void *),16,field_width,precision,flags);
					break;

				case 'x':

					flags |= SMALL;

				case 'X':

					if(qualifier == 'l')
						str = number(str,va_arg(args,unsigned long),16,field_width,precision,flags);
					else
						str = number(str,va_arg(args,unsigned int),16,field_width,precision,flags);
					break;

				case 'd':
				case 'i':

					flags |= SIGN;
				case 'u':

					if(qualifier == 'l')
						str = number(str,va_arg(args,unsigned long),10,field_width,precision,flags);
					else
						str = number(str,va_arg(args,unsigned int),10,field_width,precision,flags);
					break;

				case 'n':
					
					if(qualifier == 'l')
					{
						long *ip = va_arg(args,long *);
						*ip = (str - buf);
					}
					else
					{
						int *ip = va_arg(args,int *);
						*ip = (str - buf);
					}
					break;

				case '%':
					
					*str++ = '%';
					break;

				default:

					*str++ = '%';	
					if(*fmt)
						*str++ = *fmt;
					else
						fmt--;
					break;
			}

	}
	*str = '\0';
	return str - buf;
}

*/