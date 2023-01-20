
// address
#define UART0               (volatile unsigned int*) 0x3F201000 
#define UART0_DR            (volatile unsigned int*) 0x3F201000 
#define UART0_FR            (volatile unsigned int*) 0x3F201018 
#define UART0_IBRD          (volatile unsigned int*) 0x3F201024 
#define UART0_FBRD          (volatile unsigned int*) 0x3F201028 
#define UART0_LCRH          (volatile unsigned int*) 0x3F20102C 
#define UART0_CR            (volatile unsigned int*) 0x3F201030 
#define UART0_IMSC          (volatile unsigned int*) 0x3F201038 
#define UART0_ICR           (volatile unsigned int*) 0x3F201044 

// Set baud rate and characteristics (115200 8N1) and map to GPIO
void _init_uart0();

// Send a character
void _uart0_sendchar(unsigned int c);

// Display a string
void _uart0_sendstring(char *s);

// Display a binary value in hexadecimal
void _uart0_sendhex(unsigned int d);

// Receive a character
char _uart0_getchar();