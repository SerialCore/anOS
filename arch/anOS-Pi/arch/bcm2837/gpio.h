
// address
#define GPFSEL0             (volatile unsigned int*) 0x3F200000
#define GPFSEL1             (volatile unsigned int*) 0x3F200004
#define GPFSEL2             (volatile unsigned int*) 0x3F200008
#define GPFSEL3             (volatile unsigned int*) 0x3F20000C
#define GPFSEL4             (volatile unsigned int*) 0x3F200010
#define GPFSEL5             (volatile unsigned int*) 0x3F200014
#define GPSET0              (volatile unsigned int*) 0x3F20001C
#define GPSET1              (volatile unsigned int*) 0x3F200020
#define GPCLR0              (volatile unsigned int*) 0x3F200028
#define GPCLR1              (volatile unsigned int*) 0x3F20002C
#define GPLEV0              (volatile unsigned int*) 0x3F200034
#define GPLEV1              (volatile unsigned int*) 0x3F200038
#define GPEDS0              (volatile unsigned int*) 0x3F200040
#define GPEDS1              (volatile unsigned int*) 0x3F200044
#define GPFEN0				(volatile unsigned int*) 0x3F200058
#define GPFEN1				(volatile unsigned int*) 0x3F20005C
#define GPHEN0              (volatile unsigned int*) 0x3F200064
#define GPHEN1              (volatile unsigned int*) 0x3F200068
#define GPLEN0				(volatile unsigned int*) 0x3F200070
#define GPLEN1				(volatile unsigned int*) 0x3F200074
#define GPAREN0				(volatile unsigned int*) 0x3F20007C
#define GPAREN1				(volatile unsigned int*) 0x3F200080
#define GPAFEN0				(volatile unsigned int*) 0x3F200088
#define GPAFEN1				(volatile unsigned int*) 0x3F20008C	
#define GPPUD               (volatile unsigned int*) 0x3F200094
#define GPPUDCLK0           (volatile unsigned int*) 0x3F200098
#define GPPUDCLK1           (volatile unsigned int*) 0x3F20009C
/*
 * GPFSEL0 --- 0x7E200000 --- GPIO0-GPIO9
 * GPFSEL1 --- 0x7E200004 --- GPIO10-GPIO19
 * GPFSEL2 --- 0x7E200008 --- GPIO20-GPIO29
 * GPFSEL3 --- 0x7E20000c --- GPIO30-GPIO39
 * GPFSEL4 --- 0x7E200010 --- GPIO40-GPIO49
 * GPFSEL5 --- 0x7E200014 --- GPIO50-GPIO53
 * GPSET0 --- 0x7E20001C --- GPIO0-GPIO31
 * GPSET1 --- 0x7E200020 --- GPIO32-GPIO53
 * 
 * GPSET0 --- 0x7E20001C --- GPIO0-GPIO31
 * GPSET1 --- 0x7E200020 --- GPIO32-GPIO53
 *
 * GPCLR0 --- 0x7E200028 --- GPIO0-GPIO31
 */

typedef enum
{
	GPIO_INPUT = 0,
	GPIO_OUTPUT,
	GPIO_ALTER5,
	GPIO_ALTER4,
	GPIO_ALTER0,
	GPIO_ALTER1,
	GPIO_ALTER2,
	GPIO_ALTER3,
} gpio_func_t;

void _gpio_function(unsigned int pin, gpio_func_t function);

void _gpio_write(unsigned int pin, unsigned int value);

unsigned int _gpio_read(unsigned int pin);