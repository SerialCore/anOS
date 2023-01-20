
// address
#define UART1               (volatile unsigned int*) 0x3F215000
#define AUX_ENABLE          (volatile unsigned int*) 0x3F215004
#define AUX_MU_IO           (volatile unsigned int*) 0x3F215040
#define AUX_MU_IER          (volatile unsigned int*) 0x3F215044
#define AUX_MU_IIR          (volatile unsigned int*) 0x3F215048
#define AUX_MU_LCR          (volatile unsigned int*) 0x3F21504C
#define AUX_MU_MCR          (volatile unsigned int*) 0x3F215050
#define AUX_MU_LSR          (volatile unsigned int*) 0x3F215054
#define AUX_MU_MSR          (volatile unsigned int*) 0x3F215058
#define AUX_MU_SCRATCH      (volatile unsigned int*) 0x3F21505C
#define AUX_MU_CNTL         (volatile unsigned int*) 0x3F215060
#define AUX_MU_STAT         (volatile unsigned int*) 0x3F215064
#define AUX_MU_BAUD         (volatile unsigned int*) 0x3F215068

// Set baud rate and characteristics (115200 8N1) and map to GPIO
void _init_uart1();

// Send a character
void _uart1_sendchar(unsigned int c);

// Display a string
void _uart1_sendstring(char *s);

// Receive a character
char _uart1_getchar();